/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package services

import (
	errori "backend/internal/errors"
	"backend/models"
	"context"
	"fmt"
	"strconv"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgtype"
	"github.com/jackc/pgx/v5/pgxpool"
)

type TournamentParticipantService struct {
	db *pgxpool.Pool
}

func NewTournamentParticipantService(db *pgxpool.Pool) *TournamentParticipantService {
	return &TournamentParticipantService{db}
}

func (s *TournamentParticipantService) GetAllTournamentParticipants() ([]models.TournamentParticipant, error) {
	ctx := context.Background()
	rows, err := s.db.Query(ctx, "SELECT * from TournamentParticipant")
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var tournament_participants []models.TournamentParticipant
	for rows.Next() {
		var tournament_participant models.TournamentParticipant
		if err := rows.Scan(&tournament_participant.ID,
			&tournament_participant.State,
			&tournament_participant.TeamID,
			&tournament_participant.PlayerID,
			&tournament_participant.TournamentID); err != nil {
			return nil, err
		}
		tournament_participants = append(tournament_participants, tournament_participant)
	}

	return tournament_participants, nil
}

func (s *TournamentParticipantService) CountPlayers(searchText string) (int, error) {
	ctx := context.Background()
	total := -1
	err := s.db.QueryRow(ctx, `
		SELECT COUNT(id) FROM
		(
		SELECT tp.player_id AS id FROM TournamentParticipant tp
		JOIN "User" u ON u.id = tp.player_id
		WHERE player_id IS NOT NULL AND ($1 = '' OR similarity(u.name || ' ' || u.surname, $1) > 0.05)
		UNION
		SELECT p.user_id AS id FROM TournamentParticipant tp
			INNER JOIN Team t ON tp.team_id = t.id
			JOIN TeamPlayer p ON p.team_id = t.id
			JOIN "User" u ON p.user_id = u.id
			WHERE ($1 = '' OR similarity(u.name || ' ' || u.surname, $1) > 0.05)
		) as AllPlayers
	`, searchText).Scan(&total)
	return total, err
}

func (s *TournamentParticipantService) GetPlayers(page, limit int, searchText string) (models.PaginationAnswer[models.Player], error) {
	var ans models.PaginationAnswer[models.Player]
	ctx := context.Background()
	offset := (page - 1) * limit

	// 1. get total
	total, err := s.CountPlayers(searchText)
	if err != nil {
		return ans, err
	}

	// 2. get data
	rows, err := s.db.Query(ctx, `
		SELECT id, name, surname FROM
			(
				SELECT "User".* FROM TournamentParticipant
					INNER JOIN "User" ON "User".id = TournamentParticipant.player_id
					WHERE ($3 = '' OR similarity(name || ' ' || surname, $3) > 0.05)
				UNION
				SELECT "User".* AS id FROM TournamentParticipant
					INNER JOIN Team ON TournamentParticipant.team_id = Team.id
					JOIN TeamPlayer ON TeamPlayer.team_id = Team.id
					JOIN "User" ON "User".id = TeamPlayer.user_id
					WHERE ($3 = '' OR similarity("User".name || ' ' || "User".surname, $3) > 0.05)
			) as AllPlayers
		LIMIT $1 OFFSET $2
	`, limit, offset, searchText)
	if err != nil {
		return ans, err
	}
	defer rows.Close()

	var players []models.Player
	for rows.Next() {
		var player models.Player
		if err := rows.Scan(&player.ID,
			&player.Name,
			&player.Surname,
		); err != nil {
			return ans, err
		}
		players = append(players, player)
	}

	// 3. meta computing
	totalPages := (total + limit - 1) / limit
	ans = models.PaginationAnswer[models.Player]{
		Data:         players,
		TotalRecords: total,
		TotalPages:   totalPages,
		CurrentPage:  page,
		Limit:        limit,
	}
	return ans, nil
}

func (s *TournamentParticipantService) GetPlayerById(id int) (models.PlayerDetail, error) {
	ctx := context.Background()
	var player models.PlayerDetail

	err := s.db.QueryRow(ctx, `
		SELECT id, name, surname
		FROM "User"
		WHERE id = $1
	`, id).Scan(&player.ID, &player.Name, &player.Surname)
	if err != nil {
		if err == pgx.ErrNoRows {
			return player, nil
		}
		return player, err
	}

	return player, nil
}

func (s *TournamentParticipantService) GetPlayerWinrate(id int32) (models.WinrateStatistic, error) {
	ctx := context.Background()
	var winrate models.WinrateStatistic

	query := `
	WITH played_teams AS (
		SELECT tp.team_id, tp.since, tp.until FROM TeamPlayer tp
		WHERE tp.user_id = $1
	)
	SELECT COUNT(*) FROM Match m
	JOIN TournamentParticipant tp1 ON m.first_participant_id = tp1.id
	JOIN TournamentParticipant tp2 ON m.second_participant_id = tp2.id
	LEFT JOIN "User" u ON u.id = $1 AND (
		(u.id = tp1.player_id AND m.first_participant_is_winner = $2)
		OR (u.id = tp2.player_id AND m.second_participant_is_winner = $2))
	LEFT JOIN Team t
		ON (t.id = tp1.team_id AND t.id IN (SELECT team_id FROM played_teams) AND m.first_participant_is_winner = $2)
		OR (t.id = tp2.team_id AND t.id IN (SELECT team_id FROM played_teams) AND m.second_participant_is_winner = $2)
	WHERE (u.id IS NOT NULL or t.id IS NOT NULL)
	AND (m.first_participant_is_winner OR m.second_participant_is_winner)
	`

	err := s.db.QueryRow(ctx, query, id, true).Scan(&winrate.Wins)
	if err != nil {
		return winrate, err
	}

	err = s.db.QueryRow(ctx, query, id, false).Scan(&winrate.Loses)
	if err != nil {
		return winrate, err
	}

	return winrate, nil
}

func (s *TournamentParticipantService) GetPlayerDisciplines(id int32) ([]models.DisciplineStatistic, error) {
	ctx := context.Background()
	var disciplines []models.DisciplineStatistic

	rows, err := s.db.Query(ctx, `
	WITH played_teams AS (
		SELECT tp.team_id, tp.since, tp.until FROM TeamPlayer tp
		WHERE tp.user_id = $1
	)
	SELECT t.discipline, COUNT(*) FROM Tournament t
	JOIN TournamentParticipant tp ON tp.tournament_id = t.id
	WHERE (tp.player_id = $1 OR EXISTS (SELECT team_id FROM played_teams p WHERE p.team_id = tp.team_id))
	AND tp.state = 'Accepted'
	GROUP BY t.discipline
	`, id)

	if err == pgx.ErrNoRows {
		rows.Close()
		return []models.DisciplineStatistic{}, nil
	} else if err != nil {
		return nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var discipline models.DisciplineStatistic
		if err = rows.Scan(
			&discipline.Name,
			&discipline.Tournaments,
		); err != nil {
			return nil, err
		}
		disciplines = append(disciplines, discipline)
	}

	return disciplines, nil
}

func (s *TournamentParticipantService) GetPlayerActivity(id int32) (map[string]models.ActivityStatistic, error) {
	ctx := context.Background()
	matchmap := map[string]models.ActivityStatistic{}

	prows, err := s.db.Query(ctx, `
		SELECT COUNT(*) AS match_count, date_trunc('month', m."date") as match_month FROM Match m
		JOIN TournamentParticipant tp ON m.first_participant_id = tp.id OR m.second_participant_id = tp.id
		WHERE tp.player_id = $1 AND m."date" >= date_trunc('month', CURRENT_DATE) - INTERVAL '3 month'
		AND (m.first_participant_is_winner OR m.second_participant_is_winner)
		GROUP BY match_month
	`, id)

	if err != nil {
		return matchmap, err
	}

	for prows.Next() {
		var m_count int
		var m_time pgtype.Date
		if err := prows.Scan(&m_count, &m_time); err != nil {
			return matchmap, err
		}
		monthstat := models.ActivityStatistic{Month: m_time.Time.Month().String(), Personal: m_count, Teams: 0}
		matchmap[m_time.Time.Month().String()] = monthstat
	}
	defer prows.Close()

	trows, err := s.db.Query(ctx, `
		WITH played_teams AS (
			SELECT tp.team_id, tp.since, tp.until FROM TeamPlayer tp
			WHERE tp.user_id = $1
		)
		SELECT COUNT(*) AS match_count, date_trunc('month', m."date") as match_month FROM Match m
		JOIN TournamentParticipant tp ON m.first_participant_id = tp.id OR m.second_participant_id = tp.id
		WHERE EXISTS (SELECT pt.team_id FROM played_teams pt WHERE pt.team_id = tp.team_id)
		AND m."date" >= date_trunc('month', CURRENT_DATE) - INTERVAL '3 month'
		AND (m.first_participant_is_winner OR m.second_participant_is_winner)
		GROUP BY match_month
	`, id)

	if err != nil {
		return matchmap, err
	}
	defer trows.Close()

	for trows.Next() {
		var m_count int
		var m_time pgtype.Date
		if err := trows.Scan(&m_count, &m_time); err != nil {
			return matchmap, err
		}
		if monthstat, ok := matchmap[m_time.Time.Month().String()]; ok {
			monthstat.Teams = m_count
			matchmap[m_time.Time.Month().String()] = monthstat
		} else {
			monthstat := models.ActivityStatistic{Month: m_time.Time.Month().String(), Personal: 0, Teams: m_count}
			matchmap[m_time.Time.Month().String()] = monthstat
		}
	}

	return matchmap, nil
}

func (s *TournamentParticipantService) GetPlayerWinnings(userID int32) (int32, error) {
	ctx := context.Background()
	var winnings pgtype.Int4
	err := s.db.QueryRow(ctx, `
		SELECT SUM(prize) FROM Tournament t
		WHERE EXISTS (
			SELECT * FROM Stage s
			JOIN Match m ON m.stage_id = s.id
			JOIN TournamentParticipant tp1 ON m.first_participant_id = tp1.id
			JOIN TournamentParticipant tp2 ON m.second_participant_id = tp2.id
			WHERE s.tournament_id = t.id AND s.level = (SELECT MAX(level) FROM Stage s2 WHERE s2.tournament_id = t.id)
			AND (
				(tp1.player_id = $1 AND m.first_participant_is_winner)
				OR 
				(tp2.player_id = $1 AND m.second_participant_is_winner)
		))
	`, userID).Scan(&winnings)
	if err != nil {
		return 0, err
	}
	if winnings.Valid {
		return winnings.Int32, err
	}
	return 0, err
}

func (s *TournamentParticipantService) PlayerPaticipatesTournament(tournamentID, playerdID int32) (int32, error) {
	ctx := context.Background()
	var id int32
	err := s.db.QueryRow(ctx, `
		SELECT tp.id FROM TournamentParticipant tp
		WHERE tp.tournament_id = $1 AND tp.player_id = $2
	`, tournamentID, playerdID).Scan(&id)
	if err == pgx.ErrNoRows {
		return id, errori.DBNotFound
	}
	return id, err
}

func (s *TournamentParticipantService) TeamOrPlayerPaticipatesTournament(tournamentID, teamID int32) (int32, error) {
	ctx := context.Background()
	var id int32
	err := s.db.QueryRow(ctx, `
	SELECT tp.id FROM TournamentParticipant tp
	WHERE tp.tournament_id = $1 AND (tp.team_id = $2
  	OR EXISTS (SELECT * FROM TeamPlayer teamp
			WHERE teamp.team_id = tp.team_id AND teamp.until IS NULL AND tp.state = 'Accepted'
      AND teamp.user_id IN ( SELECT itemp.user_id FROM TeamPlayer itemp
        WHERE itemp.team_id = $2 AND itemp.until IS NULL
      )
		)
	)
	`, tournamentID, teamID).Scan(&id)
	if err == pgx.ErrNoRows {
		return id, errori.DBNotFound
	}
	return id, err
}

func (s *TournamentParticipantService) CreateTournamentParticipant(
	id string,
	req *models.CreateTournamentParticipant,
	tournamentType string,
) (models.TournamentParticipant, error) {
	ctx := context.Background()
	tournamentID, err := strconv.Atoi(id)
	var participant models.TournamentParticipant
	if err != nil {
		return participant, err
	}

	switch tournamentType {
	case "Team":
		if req.PlayerID.Valid {
			return participant, fmt.Errorf("Player must be null for this tournament")
		}
		if req.TeamID.Valid == false {
			return participant, fmt.Errorf("Team must be provided for this tournament")
		}
	case "Person":
		if req.PlayerID.Valid == false {
			return participant, fmt.Errorf("Player must be provided for this tournament")
		}
		if req.TeamID.Valid {
			return participant, fmt.Errorf("Team must be null for this tournament")
		}
	default:
		return participant, fmt.Errorf("unknown tournament type: %s", tournamentType)
	}

	if err := s.db.QueryRow(ctx, `
		INSERT INTO TournamentParticipant(team_id, player_id, tournament_id)
		VALUES ($1, $2, $3)
		RETURNING id
	`, req.TeamID, req.PlayerID, tournamentID).Scan(&participant.ID); err != nil {
		return participant, err
	}

	participant.PlayerID = req.PlayerID
	participant.TeamID = req.TeamID
	participant.State = "Pending"
	participant.TournamentID = int32(tournamentID)

	return participant, nil
}

func (s *TournamentParticipantService) ResolveTournamentParticipant(id int32, newState string) error {
	ctx := context.Background()
	tx, err := s.db.Begin(ctx)
	if err != nil {
		return err
	}

	var tourID int32
	var teamID pgtype.Int4
	err = tx.QueryRow(ctx, `
		UPDATE TournamentParticipant
		SET state = $2
		WHERE id = $1
		RETURNING tournament_id, team_id
	`, id, newState).Scan(&tourID, &teamID)
	if err != nil {
		tx.Rollback(ctx)
		return err
	}

	if newState == "Accepted" && teamID.Valid {
		_, err = tx.Exec(ctx, `
			UPDATE TournamentParticipant tp
			SET state = 'Rejected'
			WHERE tp.id <> $3 AND tp.tournament_id = $1 AND EXISTS (SELECT * FROM TeamPlayer teamp
				WHERE teamp.team_id = tp.team_id AND teamp.until IS NULL
      	AND teamp.user_id IN (SELECT itemp.user_id FROM TeamPlayer itemp
        	WHERE itemp.team_id = $2 AND itemp.until IS NULL)
			)
		`, tourID, teamID.Int32, id)
		if err != nil {
			tx.Rollback(ctx)
			return err
		}
	}

	tx.Commit(ctx)
	return err
}
