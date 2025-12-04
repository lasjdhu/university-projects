/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package services

import (
	"backend/internal/errors"
	errori "backend/internal/errors"
	"backend/models"
	"context"
	"fmt"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgtype"
	"github.com/jackc/pgx/v5/pgxpool"
)

type TeamService struct {
	db *pgxpool.Pool
}

func NewTeamService(db *pgxpool.Pool) *TeamService {
	return &TeamService{db}
}

func (s *TeamService) GetAllTeams() ([]models.Team, error) {
	ctx := context.Background()
	rows, err := s.db.Query(ctx, "SELECT * from Team")
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var teams []models.Team
	for rows.Next() {
		var team models.Team
		if err := rows.Scan(&team.ID, &team.Name, &team.ManagerID); err != nil {
			return nil, err
		}
		teams = append(teams, team)
	}

	return teams, nil
}

func (s *TeamService) countTeams(searchText, where_and string) (int, error) {
	ctx := context.Background()
	total := -1
	err := s.db.QueryRow(ctx, `
		SELECT COUNT(*) FROM Team t
		WHERE ($1 = '' OR similarity(name, $1) > 0.05)
		`+where_and, searchText).Scan(&total)
	return total, err
}

func (s *TeamService) GetTeams(page, limit, userID int, searchText string) (models.PaginationAnswer[models.Team], error) {
	var ans models.PaginationAnswer[models.Team]
	ctx := context.Background()
	offset := (page - 1) * limit

	where_and := ""
	if userID != -1 {
		where_and += fmt.Sprintf(" AND EXISTS (SELECT tp.id FROM TeamPlayer tp WHERE tp.user_id = %d AND tp.team_id = t.id AND tp.state = 'Active')", userID)
	}

	// 1. get total
	total, err := s.countTeams(searchText, where_and)
	if err != nil {
		return ans, err
	}

	// 2. get data
	rows, err := s.db.Query(ctx, `
		SELECT t.id, t.name, t.description, t.manager_id
		FROM Team t
		WHERE ($3 = '' OR similarity(t.name, $3) > 0.05)`+where_and+
		`ORDER BY CASE WHEN $3 = '' THEN id ELSE similarity(t.name, $3) END DESC
		LIMIT $1 OFFSET $2;`, limit, offset, searchText)
	if err != nil {
		return ans, err
	}
	defer rows.Close()

	var teams []models.Team
	for rows.Next() {
		var team models.Team
		if err := rows.Scan(
			&team.ID,
			&team.Name,
			&team.Description,
			&team.ManagerID,
		); err != nil {
			return ans, err
		}
		teams = append(teams, team)
	}

	// 3. meta computing
	totalPages := (total + limit - 1) / limit
	ans = models.PaginationAnswer[models.Team]{
		Data:         teams,
		TotalRecords: total,
		TotalPages:   totalPages,
		CurrentPage:  page,
		Limit:        limit,
	}
	return ans, nil
}

func (s *TeamService) GetTeamById(id int) (models.TeamBaseResponse, int32, error) {
	ctx := context.Background()
	var team models.TeamBaseResponse

	var managerID int32

	err := s.db.QueryRow(ctx, `
		SELECT id, name, description, since, manager_id
		FROM Team
		WHERE id = $1;
	`, id).Scan(&team.ID, &team.Name, &team.Description, &team.Since, &managerID)
	if err != nil {
		if err == pgx.ErrNoRows {
			return team, -1, errori.DBNotFound
		}
		return team, -1, err
	}
	return team, managerID, nil
}

func (s *TeamService) GetTeamDetailById(id int) (models.TeamDetail, error) {
	ctx := context.Background()
	var team models.TeamDetail

	teambase, managerID, err := s.GetTeamById(id)
	if managerID == -1 {
		return team, nil
	}
	if err != nil {
		return team, err
	}
	team.TeamBaseResponse = teambase

	err = s.db.QueryRow(ctx, `
		SELECT id, name, surname
		FROM "User"
		WHERE id = $1
	`, managerID).Scan(&team.Manager.ID, &team.Manager.Name, &team.Manager.Surname)
	if err != nil {
		return team, err
	}

	playerRows, err := s.db.Query(ctx, `
		SELECT DISTINCT u.id, u.name, u.surname
		FROM TeamPlayer tp
		JOIN "User" u ON u.id = tp.user_id
		WHERE tp.team_id = $1 and tp.until IS NULL
	`, id)
	if err != nil {
		return team, err
	}
	defer playerRows.Close()

	for playerRows.Next() {
		var p models.Player
		if err := playerRows.Scan(&p.ID, &p.Name, &p.Surname); err != nil {
			return team, err
		}
		team.Players = append(team.Players, p)
	}

	if team.ID == 0 {
		return team, &errors.ErrNotFound
	}

	return team, nil
}

func (s *TeamService) GetTeamsProfile(id int32, searchText string) ([]models.TeamAndPlayers, error) {
	ctx := context.Background()
	var teams []models.TeamAndPlayers

	teamsRows, err := s.db.Query(ctx, `
		SELECT id, name, since FROM Team
		WHERE manager_id = $1 AND ($2 = '' OR similarity(name, $2) > 0.05)
	`, id, searchText)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}
	defer teamsRows.Close()

	for teamsRows.Next() {
		var t models.TeamAndPlayers
		if err := teamsRows.Scan(
			&t.ID,
			&t.Name,
			&t.Since,
		); err != nil {
			return teams, err
		}

		playersRows, err := s.db.Query(ctx, `
			SELECT tp.id, u.id, u.name, u.surname, tp.state
			FROM TeamPlayer tp
			JOIN "User" u ON u.id = tp.user_id
			WHERE tp.team_id = $1
			AND tp.until IS NULL
		`, t.ID)

		if err != nil {
			teams = append(teams, t)
			continue
		}
		defer playersRows.Close()

		for playersRows.Next() {
			var p models.TeamUserPlayer
			if err := playersRows.Scan(
				&p.PlayerID,
				&p.UserID,
				&p.Name,
				&p.Surname,
				&p.State,
			); err != nil {
				break
			}
			t.Players = append(t.Players, p)
		}

		teams = append(teams, t)
	}

	for i, team := range teams {
		count := len(team.Players)
		count = count - 1
		team.ConfilctTournaments = []string{}

		confrows, err := s.db.Query(ctx, `
			SELECT t.name FROM Tournament t
			WHERE EXISTS (SELECT * FROM TournamentParticipant tp WHERE tp.tournament_id = t.id AND tp.team_id = $1) AND
			NOT EXISTS (SELECT * FROM Stage s WHERE s.tournament_id = t.id) AND
			t.min_team_limit IS NOT NULL AND t.min_team_limit > $2
		`, team.ID, count)

		if err != nil {
			return teams, err
		}
		defer confrows.Close()

		for confrows.Next() {
			var conflict string
			if err := confrows.Scan(&conflict); err != nil {
				return teams, err
			}
			team.ConfilctTournaments = append(team.ConfilctTournaments, conflict)
		}
		teams[i] = team
	}

	return teams, nil
}

func (s *TeamService) GetTeamWinnings(teamID int32) (int32, error) {
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
				(tp1.team_id = $1 AND m.first_participant_is_winner)
				OR
				(tp2.team_id = $1 AND m.second_participant_is_winner)
		))
	`, teamID).Scan(&winnings)
	if err != nil {
		return 0, err
	}
	if winnings.Valid {
		return winnings.Int32, err
	}
	return 0, err
}

func (s *TeamService) CreateTeamWithInvites(name, description string, managerID int32, invites []string) (models.Team, error) {
	ctx := context.Background()
	var team models.Team

	transaction, err := s.db.Begin(ctx)
	if err != nil {
		return team, err
	}
	defer transaction.Rollback(ctx)

	row := transaction.QueryRow(ctx, `
		INSERT INTO Team(name, since, description, manager_id) VALUES($1, CURRENT_DATE, $2, $3)
		RETURNING id, name, since, manager_id
	`, name, description, managerID)

	err = row.Scan(&team.ID, &team.Name, &team.Since, &team.ManagerID)
	if err != nil {
		return team, err
	}

	for i := range invites {
		email := invites[i]
		row = transaction.QueryRow(ctx, `
		SELECT id FROM "User" WHERE email = $1
		`, email)

		var userID int32
		err = row.Scan(&userID)
		if err != nil {
			return team, err
		}

		_, err = transaction.Exec(ctx, `
		INSERT INTO TeamPlayer(user_id, team_id) VALUES($1, $2)
		RETURNING *
		`, userID, team.ID)
		if err != nil {
			return team, err
		}
	}

	if err = transaction.Commit(ctx); err != nil {
		return team, err
	}
	return team, nil
}

func (s *TeamService) UpdateTeam(teamID int32, name string, desc string) error {
	ctx := context.Background()

	_, err := s.db.Exec(ctx, `
		UPDATE Team
		SET name = $2, description = $3
		WHERE id = $1
	`, teamID, name, desc)

	return err
}

func (s *TeamService) GetTeamWinrate(id int32) (models.WinrateStatistic, error) {
	ctx := context.Background()
	var winrate models.WinrateStatistic

	err := s.db.QueryRow(ctx, `
	SELECT
		COUNT(*) FILTER (WHERE
			(tp1.team_id = $1 AND m.first_participant_is_winner = TRUE) OR
			(tp2.team_id = $1 AND m.second_participant_is_winner = TRUE)
		) AS wins,
		COUNT(*) FILTER (WHERE
			(tp1.team_id = $1 AND m.first_participant_is_winner = FALSE) OR
			(tp2.team_id = $1 AND m.second_participant_is_winner = FALSE)
		) AS loses
	FROM Match m
	JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
	JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
	WHERE (m.first_participant_is_winner OR m.second_participant_is_winner)
	`, id).Scan(&winrate.Wins, &winrate.Loses)

	return winrate, err
}

func (s *TeamService) GetTeamDisciplines(id int32) ([]models.DisciplineStatistic, error) {
	ctx := context.Background()
	var disciplines []models.DisciplineStatistic

	rows, err := s.db.Query(ctx, `
	SELECT t.discipline, COUNT(*) FROM Tournament t
	JOIN TournamentParticipant tp ON tp.tournament_id = t.id
	WHERE tp.state = 'Accepted' AND tp.team_id = $1
	GROUP BY t.discipline
	`, id)

	if err != nil {
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

func (s *TeamService) GetTeamActivity(id int32) (map[string]models.ActivityStatistic, error) {
	ctx := context.Background()
	matchmap := map[string]models.ActivityStatistic{}

	rows, err := s.db.Query(ctx, `
		SELECT COUNT(*) AS match_count, date_trunc('month', m."date") as match_month FROM Match m
		JOIN TournamentParticipant tp1 ON m.first_participant_id = tp1.id
		JOIN TournamentParticipant tp2 ON m.second_participant_id = tp2.id
		WHERE (tp1.team_id = $1 OR tp2.team_id = $1) AND m."date" >= date_trunc('month', CURRENT_DATE) - INTERVAL '3 month'
		AND (m.first_participant_is_winner OR m.second_participant_is_winner)
		GROUP BY match_month
	`, id)

	if err != nil {
		return matchmap, err
	}
	defer rows.Close()

	for rows.Next() {
		var m_count int
		var m_time pgtype.Date
		if err := rows.Scan(&m_count, &m_time); err != nil {
			return matchmap, err
		}
		monthstat := models.ActivityStatistic{Month: m_time.Time.Month().String(), Personal: 0, Teams: m_count}
		matchmap[m_time.Time.Month().String()] = monthstat
	}

	return matchmap, err
}

func (s *TeamService) CountActiveTeamPlayers(teamID int32) int32 {
	ctx := context.Background()
	var count int32
	err := s.db.QueryRow(ctx, `
		SELECT COUNT(*) FROM Team t
		JOIN TeamPlayer tp ON t.id = tp.team_id
		WHERE t.id = $1 AND tp.since IS NOT NULL AND tp.until IS NULL
	`, teamID).Scan(&count)
	if err != nil {
		return -1
	}
	return count
}
