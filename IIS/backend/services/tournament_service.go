/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package services

import (
	"backend/internal/errors"
	errori "backend/internal/errors"
	"backend/models"
	"context"
	"database/sql"
	"fmt"
	"log"
	"strconv"
	"strings"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgtype"

	"github.com/jackc/pgx/v5/pgxpool"
)

type TournamentService struct {
	db *pgxpool.Pool
}

func NewTournamentService(db *pgxpool.Pool) *TournamentService {
	return &TournamentService{db}
}

func (s *TournamentService) CountTournaments(state, searchText, with, where_and string) (int, error) {
	ctx := context.Background()
	var total int
	err := s.db.QueryRow(ctx, with+`
		SELECT COUNT(*) FROM Tournament t
		WHERE ($1 = '' OR state=$1) AND ($2 = '' OR similarity(name, $2) > 0.05)
	`+where_and, state, searchText).Scan(&total)
	return total, err
}

func (s *TournamentService) GetTournaments(page, limit, teamID, userID int, searchText string) (models.PaginationAnswer[models.TournamentBaseResponse], error) {
	var ans models.PaginationAnswer[models.TournamentBaseResponse]
	ctx := context.Background()
	offset := (page - 1) * limit

	with := ""
	where_and := ""
	if teamID != -1 {
		where_and += fmt.Sprintf(" AND EXISTS (SELECT tp.id FROM TournamentParticipant tp WHERE tp.team_id = %d AND tp.tournament_id = t.id) ", teamID)
	}
	if userID != -1 {
		with += fmt.Sprintf(`WITH participated AS (
			SELECT tp.id, t.id AS tid, t.name AS tname FROM TournamentParticipant tp
			JOIN Tournament t ON tp.tournament_id = t.id
			WHERE
      (tp.player_id IS NOT NULL AND tp.player_id = %d)
      OR
      (tp.team_id IS NOT NULL AND %d IN (SELECT user_id FROM TeamPlayer p WHERE p.team_id = tp.team_id))
		)`, userID, userID)
		where_and += " AND EXISTS (SELECT p.id FROM participated p WHERE p.tid = t.id) "
	}

	total, err := s.CountTournaments("Accepted", searchText, with, where_and)
	if err != nil {
		return ans, err
	}

	rows, err := s.db.Query(ctx, with+`
		SELECT t.id, t.name, t.discipline, t.expected_members, t.type
		FROM Tournament t
		WHERE state = 'Accepted' AND ($3 = '' OR similarity(t.name, $3) > 0.05)`+
		where_and+
		`ORDER BY CASE WHEN $3 = '' THEN id ELSE similarity(t.name, $3) END DESC
		LIMIT $1 OFFSET $2`, limit, offset, searchText)
	if err != nil {
		return ans, err
	}
	defer rows.Close()

	var list []models.TournamentBaseResponse
	for rows.Next() {
		var t models.TournamentBaseResponse
		if err := rows.Scan(
			&t.ID,
			&t.Name,
			&t.Discipline,
			&t.ExpectedMembers,
			&t.Type,
		); err != nil {
			return ans, err
		}
		list = append(list, t)
	}

	ans = models.PaginationAnswer[models.TournamentBaseResponse]{
		Data:         list,
		TotalRecords: total,
		TotalPages:   (total + limit - 1) / limit,
		CurrentPage:  page,
		Limit:        limit,
	}
	return ans, nil
}

func (s *TournamentService) GetTournamentById(tID int32) (*models.TournamentDetailed, error) {
	ctx := context.Background()

	row := s.db.QueryRow(ctx, `
		SELECT t.id, t.name, t.discipline, t.expected_members, t.type, t.prize, t.min_team_limit, t.max_team_limit,
		       u.id, u.name, u.surname, t.state
		FROM Tournament t
		JOIN "User" u ON u.id = t.manager_id
		WHERE t.id = $1
	`, tID)

	var dto models.TournamentDetailed
	var state string
	var prize pgtype.Int4
	var min_limit pgtype.Int4
	var max_limit pgtype.Int4
	if err := row.Scan(
		&dto.ID,
		&dto.Name,
		&dto.Discipline,
		&dto.ExpectedMembers,
		&dto.Type,
		&prize,
		&min_limit,
		&max_limit,
		&dto.Manager.ID,
		&dto.Manager.Name,
		&dto.Manager.Surname,
		&state,
	); err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}

	if prize.Valid {
		dto.Prize = prize.Int32
	} else {
		dto.Prize = 0
	}
	if max_limit.Valid {
		dto.MaxLimit = max_limit.Int32
	}
	if min_limit.Valid {
		dto.MinLimit = min_limit.Int32
	}

	var participants []models.TournamentParticipantMinimal
	rows, err := s.db.Query(ctx, `
	SELECT tp.id,
	       tp.player_id,
	       tp.team_id,
	       COALESCE(t.name, u.name || ' ' || u.surname) AS name
	FROM TournamentParticipant tp
	LEFT JOIN "User" u ON u.id = tp.player_id
	LEFT JOIN Team t ON t.id = tp.team_id
	WHERE tp.tournament_id = $1 AND tp.state = 'Accepted'
`, tID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var participant models.TournamentParticipantMinimal
		if err := rows.Scan(
			&participant.ID,
			&participant.PlayerID,
			&participant.TeamID,
			&participant.Name,
		); err != nil {
			return nil, err
		}
		participants = append(participants, participant)
	}
	if err := rows.Err(); err != nil {
		return nil, err
	}

	dto.Participants = participants

	if state != "Accepted" {
		return nil, errori.DBNotFound
	}

	return &dto, nil
}

func (s *TournamentService) CreateTournament(req *models.CreateTournamentRequest, managerID int32) (*models.Tournament, error) {
	ctx := context.Background()

	var tournament models.Tournament
	err := s.db.QueryRow(ctx, `
		INSERT INTO Tournament (name, discipline, expected_members, manager_id, type, prize, min_team_limit, max_team_limit)
		VALUES ($1, $2, $3, $4, $5, $6, $7, $8)
		RETURNING id, state
	`, req.Name, req.Discipline, req.ExpectedMembers, managerID, req.Type, req.Prize, req.MinLimit, req.MaxLimit).Scan(&tournament.ID, &tournament.State)
	if err != nil {
		return nil, err
	}

	tournament.Name = req.Name
	tournament.Discipline = req.Discipline
	tournament.ExpectedMembers = req.ExpectedMembers
	tournament.Type = req.Type
	tournament.ManagerID = managerID

	return &tournament, nil
}

func (s *TournamentService) UpdateTournament(id int32, req *models.CreateTournamentRequest) (*models.Tournament, error) {
	ctx := context.Background()

	var updatedTournament models.Tournament
	err := s.db.QueryRow(ctx, `
		UPDATE Tournament
		SET name = $1,
		    discipline = $2,
		    expected_members = $3,
		    type = $4
		WHERE id = $5
		RETURNING id, manager_id, state
	`, req.Name, req.Discipline, req.ExpectedMembers, req.Type, id).Scan(
		&updatedTournament.ID,
		&updatedTournament.ManagerID,
		&updatedTournament.State,
	)
	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}

	updatedTournament.Name = req.Name
	updatedTournament.Discipline = req.Discipline
	updatedTournament.ExpectedMembers = req.ExpectedMembers
	updatedTournament.Type = req.Type

	return &updatedTournament, nil
}

func (s *TournamentService) CanDelete(mId, tId int32) (bool, error) {
	ctx := context.Background()
	var managerID pgtype.Int4
	var state string
	err := s.db.QueryRow(ctx, `
		SELECT manager_id, state
		FROM Tournament
		WHERE id = $1
	`, tId).Scan(&managerID, &state)
	if err != nil {
		if err == pgx.ErrNoRows {
			return false, errors.ErrNotFound
		}
		return false, err
	}

	if !managerID.Valid {
		return false, nil
	}

	if managerID.Int32 != mId {
		return false, nil
	}

	return state == "Rejected", nil
}

func (s *TournamentService) DeleteTournament(id int32) error {
	ctx := context.Background()

	var deletedID int32
	err := s.db.QueryRow(ctx, `
		DELETE FROM Tournament
		WHERE id = $1
		RETURNING id
	`, id).Scan(&deletedID)
	if err != nil {
		if err == pgx.ErrNoRows {
			return errors.ErrNotFound
		}
		return err
	}

	return nil
}

func (s *TournamentService) GetTournamentBracket(id string) (*models.TournamentBracket, error) {
	ctx := context.Background()
	tournamentID, err := strconv.Atoi(id)
	if err != nil {
		return nil, err
	}

	rows, err := s.db.Query(ctx, `
		SELECT
			m.id, m.next_match_id, m.name, s.level, m."date",
			m.first_participant_id,
			m.first_participant_result_text,
			m.first_participant_is_winner,
			COALESCE(t1.name, u1.name || ' ' || u1.surname) AS first_participant_name,
			m.second_participant_id,
			m.second_participant_result_text,
			m.second_participant_is_winner,
			COALESCE(t2.name, u2.name || ' ' || u2.surname) AS second_participant_name
		FROM Match m
		INNER JOIN Stage s ON s.id = m.stage_id
		LEFT JOIN TournamentParticipant p1 ON p1.id = m.first_participant_id
		LEFT JOIN TournamentParticipant p2 ON p2.id = m.second_participant_id
		LEFT JOIN Team t1 ON t1.id = p1.team_id
		LEFT JOIN Team t2 ON t2.id = p2.team_id
		LEFT JOIN "User" u1 ON u1.id = p1.player_id
		LEFT JOIN "User" u2 ON u2.id = p2.player_id
		WHERE s.tournament_id = $1
		ORDER BY s.id, m.id
	`, tournamentID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var matches []models.BracketMatch
	for rows.Next() {
		var (
			matchID     int64
			nextMatchID pgtype.Int4
			matchName   string
			level       int32
			stageRound  string
			date        pgtype.Timestamp

			firstParticipantID  pgtype.Int4
			firstResultText     sql.NullString
			firstIsWinner       sql.NullBool
			firstName           sql.NullString
			secondParticipantID pgtype.Int4
			secondResultText    sql.NullString
			secondIsWinner      sql.NullBool
			secondName          sql.NullString
		)

		if err := rows.Scan(
			&matchID,
			&nextMatchID,
			&matchName,
			&level,
			&date,
			&firstParticipantID,
			&firstResultText,
			&firstIsWinner,
			&firstName,
			&secondParticipantID,
			&secondResultText,
			&secondIsWinner,
			&secondName,
		); err != nil {
			return nil, err
		}

		firstNameTrimmed := strings.TrimSpace(firstName.String)
		var firstNamePtr *string
		if firstNameTrimmed != "" {
			name := firstNameTrimmed
			firstNamePtr = &name
		}

		secondNameTrimmed := strings.TrimSpace(secondName.String)
		var secondNamePtr *string
		if secondNameTrimmed != "" {
			name := secondNameTrimmed
			secondNamePtr = &name
		}

		firstResultTrimmed := strings.TrimSpace(firstResultText.String)
		var firstResultPtr *string
		if firstResultTrimmed != "" {
			result := firstResultTrimmed
			firstResultPtr = &result
		}

		secondResultTrimmed := strings.TrimSpace(secondResultText.String)
		var secondResultPtr *string
		if secondResultTrimmed != "" {
			result := secondResultTrimmed
			secondResultPtr = &result
		}

		firstParticipant := models.MatchParticipant{
			ID:         firstParticipantID,
			ResultText: firstResultPtr,
			IsWinner:   firstIsWinner.Valid && firstIsWinner.Bool,
			Name:       firstNamePtr,
		}

		secondParticipant := models.MatchParticipant{
			ID:         secondParticipantID,
			ResultText: secondResultPtr,
			IsWinner:   secondIsWinner.Valid && secondIsWinner.Bool,
			Name:       secondNamePtr,
		}

		stageRound = strconv.Itoa(int(level))

		match := models.BracketMatch{
			ID:                  matchID,
			Name:                matchName,
			NextMatchID:         nextMatchID,
			TournamentRoundText: stageRound,
			Date:                date,
			Participants:        []models.MatchParticipant{firstParticipant, secondParticipant},
		}
		matches = append(matches, match)
	}

	if err := rows.Err(); err != nil {
		return nil, err
	}

	bracket := &models.TournamentBracket{
		Matches: matches,
	}
	return bracket, nil
}

func (s *TournamentService) StartTournament(id string) error {
	ctx := context.Background()
	tournamentID, err := strconv.Atoi(id)
	if err != nil {
		return err
	}

	tx, err := s.db.Begin(ctx)
	if err != nil {
		return err
	}
	defer tx.Rollback(ctx)

	var alreadyStarted bool
	if err := tx.QueryRow(ctx, `
		SELECT EXISTS (
			SELECT 1
			FROM Stage
			WHERE tournament_id = $1
		)
	`, tournamentID).Scan(&alreadyStarted); err != nil {
		return err
	}
	if alreadyStarted {
		return fmt.Errorf("tournament %d already has generated stages", tournamentID)
	}

	rows, err := tx.Query(ctx, `
		SELECT id, state, team_id, player_id, tournament_id
		FROM TournamentParticipant
		WHERE tournament_id = $1
			AND state = 'Accepted'
		ORDER BY id
	`, tournamentID)
	if err != nil {
		return err
	}
	defer rows.Close()

	var participants []models.TournamentParticipant
	for rows.Next() {
		var participant models.TournamentParticipant
		if err := rows.Scan(
			&participant.ID,
			&participant.State,
			&participant.TeamID,
			&participant.PlayerID,
			&participant.TournamentID,
		); err != nil {
			return err
		}

		participants = append(participants, participant)
	}

	if err := rows.Err(); err != nil {
		return err
	}

	if len(participants) < 2 {
		return fmt.Errorf("tournament %d requires at least two accepted participants", tournamentID)
	}
	if !isPowerOfTwo(len(participants)) {
		return fmt.Errorf("tournament %d requires a power-of-two number of accepted participants to build the bracket", tournamentID)
	}

	if err := s.createMatches(ctx, tx, tournamentID, participants); err != nil {
		return err
	}

	return tx.Commit(ctx)
}

func (s *TournamentService) createMatches(ctx context.Context, tx pgx.Tx, tournamentID int, participants []models.TournamentParticipant) error {
	totalParticipants := len(participants)
	if totalParticipants < 2 {
		return fmt.Errorf("cannot create bracket for tournament %d without participants", tournamentID)
	}

	round := 1
	matchCounter := 1

	var stageID int32
	if err := tx.QueryRow(ctx, `
		INSERT INTO Stage (tournament_id, level)
		VALUES ($1, $2)
		RETURNING id
	`, tournamentID, round).Scan(&stageID); err != nil {
		return err
	}

	currentRoundMatchIDs := make([]int32, 0, totalParticipants/2)
	for i := 0; i < totalParticipants; i += 2 {
		matchName := fmt.Sprintf("Match %d", matchCounter)
		firstParticipantID := participants[i].ID
		secondParticipantID := participants[i+1].ID

		var matchID int32
		if err := tx.QueryRow(ctx, `
			INSERT INTO Match (
				stage_id,
				name,
				first_participant_id,
				second_participant_id
			)
			VALUES ($1, $2, $3, $4)
			RETURNING id
		`, stageID, matchName, firstParticipantID, secondParticipantID).Scan(&matchID); err != nil {
			return err
		}

		currentRoundMatchIDs = append(currentRoundMatchIDs, matchID)
		matchCounter++
	}

	for len(currentRoundMatchIDs) > 1 {
		round++

		if err := tx.QueryRow(ctx, `
			INSERT INTO Stage (tournament_id, level)
			VALUES ($1, $2)
			RETURNING id
		`, tournamentID, round).Scan(&stageID); err != nil {
			return err
		}

		nextRoundMatchIDs := make([]int32, 0, len(currentRoundMatchIDs)/2)
		for i := 0; i < len(currentRoundMatchIDs); i += 2 {
			matchName := fmt.Sprintf("Match %d", matchCounter)
			var matchID int32

			if err := tx.QueryRow(ctx, `
				INSERT INTO Match (stage_id, name)
				VALUES ($1, $2)
				RETURNING id
			`, stageID, matchName).Scan(&matchID); err != nil {
				return err
			}

			if _, err := tx.Exec(ctx, `
				UPDATE Match
				SET next_match_id = $1
				WHERE id = $2 OR id = $3
			`, matchID, currentRoundMatchIDs[i], currentRoundMatchIDs[i+1]); err != nil {
				return err
			}

			nextRoundMatchIDs = append(nextRoundMatchIDs, matchID)
			matchCounter++
		}

		currentRoundMatchIDs = nextRoundMatchIDs
	}

	return nil
}

func isPowerOfTwo(n int) bool {
	return n > 0 && (n&(n-1)) == 0
}

func (s *TournamentService) GetTournamentsDetailed(
	page, limit int, state, searchText string,
) (models.PaginationAnswer[models.TournamentAdminDetailed], error) {

	var ans models.PaginationAnswer[models.TournamentAdminDetailed]
	ctx := context.Background()
	offset := (page - 1) * limit

	total, err := s.CountTournaments(state, "", "", "")
	if err != nil {
		return ans, err
	}

	rows, err := s.db.Query(ctx, `
    SELECT
        t.id, t.name, t.discipline, t.expected_members,
        t.type, t.state, t.prize, t.min_team_limit, t.max_team_limit,
        u.id, u.name, u.surname
    FROM Tournament t
    JOIN "User" u ON t.manager_id = u.id
		WHERE ($3 = '' OR t.state=$3) AND ($4 = '' OR similarity(t.name, $4) > 0.05)
    ORDER BY CASE WHEN $4 = '' THEN t.id ELSE similarity(t.name, $4) END DESC
    LIMIT $1 OFFSET $2
		`, limit, offset, state, searchText)
	if err != nil {
		return ans, err
	}
	defer rows.Close()

	var list []models.TournamentAdminDetailed
	for rows.Next() {
		var prize pgtype.Int4
		var min_limit pgtype.Int4
		var max_limit pgtype.Int4
		var t models.TournamentAdminDetailed
		if err := rows.Scan(
			&t.ID, &t.Name, &t.Discipline, &t.ExpectedMembers,
			&t.Type, &t.State, &prize, &min_limit, &max_limit,
			&t.Manager.ID, &t.Manager.Name, &t.Manager.Surname,
		); err != nil {
			return ans, err
		}
		if prize.Valid {
			t.Prize = prize.Int32
		} else {
			t.Prize = 0
		}
		if max_limit.Valid {
			t.MaxLimit = max_limit.Int32
		}
		if min_limit.Valid {
			t.MinLimit = min_limit.Int32
		}

		list = append(list, t)
	}

	ans = models.PaginationAnswer[models.TournamentAdminDetailed]{
		Data:         list,
		TotalRecords: total,
		TotalPages:   (total + limit - 1) / limit,
		CurrentPage:  page,
		Limit:        limit,
	}
	return ans, nil
}

func (s *TournamentService) GetTournamentsByManagerId(
	managerID int32,
) ([]models.TournamentAdminDetailed, error) {

	ctx := context.Background()

	rows, err := s.db.Query(ctx, `
        SELECT
            t.id, t.name, t.discipline, t.expected_members,
            t.type, t.state,
            u.id, u.name, u.surname
        FROM Tournament t
        JOIN "User" u ON t.manager_id = u.id
        WHERE t.manager_id = $1
        ORDER BY t.id DESC`,
		managerID,
	)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var tournaments []models.TournamentAdminDetailed
	for rows.Next() {
		var t models.TournamentAdminDetailed
		if err := rows.Scan(
			&t.ID, &t.Name, &t.Discipline, &t.ExpectedMembers,
			&t.Type, &t.State,
			&t.Manager.ID, &t.Manager.Name, &t.Manager.Surname,
		); err != nil {
			return nil, err
		}
		tournaments = append(tournaments, t)
	}

	if len(tournaments) == 0 {
		return nil, errori.DBNotFound
	}

	return tournaments, nil
}

func (s *TournamentService) UpdateTournamentState(id int32, state string) error {
	ctx := context.Background()
	_, err := s.db.Exec(ctx, `
		UPDATE Tournament
		SET state=$1
		WHERE id=$2`, state, id)

	if err != nil {
		if err == pgx.ErrNoRows {
			return errori.DBNotFound
		}
		return err
	}

	return nil
}

func (s *TournamentService) GetRequestsWithConflicts(ctx context.Context, tID int32) ([]models.TournamentParticipantConflictsMinimal, error) {
	var requests []models.TournamentParticipantConflictsMinimal
	rows, err := s.db.Query(ctx, `
	SELECT tp.id,
	       tp.player_id,
	       tp.team_id,
	       COALESCE(t.name, u.name || ' ' || u.surname) AS name
	FROM TournamentParticipant tp
	LEFT JOIN "User" u ON u.id = tp.player_id
	LEFT JOIN Team t ON t.id = tp.team_id
	WHERE tp.tournament_id = $1 AND tp.state = 'Pending'
	`, tID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var tpc models.TournamentParticipantConflictsMinimal
		if err := rows.Scan(
			&tpc.ID,
			&tpc.PlayerID,
			&tpc.TeamID,
			&tpc.Name,
		); err != nil {
			return nil, err
		}
		if tpc.TeamID.Valid {
			log.Println(tID, tpc.TeamID.Int32)
			confrows, err := s.db.Query(ctx, `
			SELECT DISTINCT t.name FROM TournamentParticipant tp
			JOIN Team t ON t.id = tp.team_id
			JOIN TeamPlayer teamp ON teamp.team_id = t.id
			WHERE tp.tournament_id = $1 AND t.id <> $2
			AND teamp.user_id IN (SELECT teamp2.user_id FROM TeamPlayer teamp2 WHERE teamp2.team_id = $2)
			`, tID, tpc.TeamID.Int32)
			if err == nil {
				for confrows.Next() {
					var conflict string
					if err := confrows.Scan(&conflict); err != nil {
						break
					}
					tpc.Conflicts = append(tpc.Conflicts, conflict)
				}
			}
		}
		requests = append(requests, tpc)
	}

	return requests, nil
}

func (s *TournamentService) GetTournamentsProfile(managerID int32, searchText string) ([]models.TournamentProfileDetailed, error) {
	ctx := context.Background()

	rows, err := s.db.Query(ctx, `
        SELECT
            t.id, t.name, t.discipline, t.expected_members,
            t.type, t.state, t.prize, t.min_team_limit, t.max_team_limit
        FROM Tournament t
        JOIN "User" u ON t.manager_id = u.id
        WHERE t.manager_id = $1 AND ($2 = '' OR similarity(t.name, $2) > 0.05)
        ORDER BY CASE WHEN $2 = '' THEN t.id ELSE similarity(t.name, $2) END DESC`,
		managerID, searchText,
	)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	var tournaments []models.TournamentProfileDetailed
	for rows.Next() {
		var t models.TournamentProfileDetailed
		var prize pgtype.Int4
		var min_limit pgtype.Int4
		var max_limit pgtype.Int4
		if err := rows.Scan(
			&t.ID, &t.Name, &t.Discipline, &t.ExpectedMembers,
			&t.Type, &t.State, &prize, &min_limit, &max_limit,
		); err != nil {
			return nil, err
		}
		t.RequestedParticipants, err = s.GetRequestsWithConflicts(ctx, t.ID)
		if err != nil && err != pgx.ErrNoRows {
			return nil, err
		}
		if prize.Valid {
			t.Prize = prize.Int32
		} else {
			t.Prize = 0
		}
		if max_limit.Valid {
			t.MaxLimit = max_limit.Int32
		}
		if min_limit.Valid {
			t.MinLimit = min_limit.Int32
		}
		tournaments = append(tournaments, t)
	}

	if len(tournaments) == 0 {
		return nil, errori.DBNotFound
	}

	return tournaments, nil
}

func (s *TournamentService) CheckTournamentBracket(id string, matches *models.TournamentBracket, manager int32, exists bool) error {
	ctx := context.Background()
	_, err := strconv.Atoi(id)
	if err != nil {
		return fmt.Errorf("Invalid tournament ID")
	}

	if !exists {
		return fmt.Errorf("Unathorized, cannot change matches")
	}

	for _, m := range matches.Matches {
		var fr *string
		var fw bool
		var sr *string
		var sw bool

		fp := m.Participants[0]
		sp := m.Participants[1]

		err := s.db.QueryRow(ctx, `
			SELECT first_participant_result_text, first_participant_is_winner, second_participant_result_text, second_participant_is_winner
			FROM Match
			WHERE id = $1
		`, m.ID).Scan(&fr, &fw, &sr, &sw)
		if err != nil {
			return fmt.Errorf("Cannot find editing match")
		}

		if fw || sw {
			if fw != fp.IsWinner || sw != sp.IsWinner {
				return fmt.Errorf("You cannot change match result when winner has been already entered")
			}
			if (fp.ResultText != nil && fr == nil) || (fp.ResultText == nil && fr != nil) ||
				(sp.ResultText != nil && sr == nil) || (sp.ResultText == nil && sr != nil) ||
				(sp.ResultText != nil && sr != nil && *sp.ResultText != *sr) ||
				(fp.ResultText != nil && fr != nil && *fp.ResultText != *fr) {
				return fmt.Errorf("You cannot change match result when winner has been already entered")
			}
		} else {
			if sp.IsWinner && fp.IsWinner {
				return fmt.Errorf("There must be only one winner")
			}
			if sp.IsWinner || fp.IsWinner {
				if sp.ResultText == nil || fp.ResultText == nil {
					return fmt.Errorf("Match result must be also specified when the winner is specified")
				}
				if (sp.ResultText != nil && *sp.ResultText == "") || (fp.ResultText != nil && *fp.ResultText == "") {
					return fmt.Errorf("Match result must be also specified when the winner is specified")
				}
			}
		}
	}

	return nil
}
func (s *TournamentService) UpdateTournamentBracket(id string, matches *models.TournamentBracket) (models.TournamentBracket, error) {
	ctx := context.Background()
	tournamentID, err := strconv.Atoi(id)
	if err != nil {
		return models.TournamentBracket{}, err
	}

	tx, err := s.db.Begin(ctx)
	if err != nil {
		return models.TournamentBracket{}, err
	}
	defer tx.Rollback(ctx)

	prevWinners := make(map[int32][]models.MatchResult)

	for _, match := range matches.Matches {
		if len(match.Participants) < 2 {
			return models.TournamentBracket{}, fmt.Errorf("match %d is missing participants", match.ID)
		}

		first := match.Participants[0]
		second := match.Participants[1]

		var mid int32
		var next_match_id pgtype.Int4
		var fid pgtype.Int4
		var sid pgtype.Int4
		var fwinner bool
		var swinner bool
		err = tx.QueryRow(ctx, `
			UPDATE Match
			SET
				name = $1,
				"date"=$2,
				first_participant_id = $3,
				first_participant_result_text = $4,
				first_participant_is_winner = $5,
				second_participant_id = $6,
				second_participant_result_text = $7,
				second_participant_is_winner = $8
			WHERE id = $9
			  AND stage_id IN (
				  SELECT id FROM Stage WHERE tournament_id = $10
			  )
			RETURNING id, next_match_id, first_participant_id, first_participant_is_winner, second_participant_id, second_participant_is_winner
		`,
			match.Name,
			match.Date,
			first.ID,
			first.ResultText,
			first.IsWinner,
			second.ID,
			second.ResultText,
			second.IsWinner,
			match.ID,
			tournamentID,
		).Scan(&mid, &next_match_id, &fid, &fwinner, &sid, &swinner)

		if err != nil {
			return models.TournamentBracket{}, err
		}
		if next_match_id.Valid && sid.Valid && fid.Valid && (fwinner || swinner) {
			winnerID := fid.Int32
			if swinner {
				winnerID = sid.Int32
			}

			pair, ok := prevWinners[next_match_id.Int32]
			if ok {
				pair = append(pair, models.MatchResult{WinnerID: winnerID, MatchID: mid})
				prevWinners[next_match_id.Int32] = pair
			} else {
				prevWinners[next_match_id.Int32] = []models.MatchResult{{WinnerID: winnerID, MatchID: mid}}
			}
		}
	}

	for key, value := range prevWinners {
		if len(value) == 1 {
			fp := value[0]
			var sp models.MatchResult
			var mid int32
			var fid pgtype.Int4
			var sid pgtype.Int4
			var fwinner bool
			var swinner bool
			err := tx.QueryRow(ctx, `
			SELECT id, first_participant_id, first_participant_is_winner, second_participant_id, second_participant_is_winner
			FROM Match WHERE next_match_id = $1 AND id <> $2
			`, key, fp.MatchID).Scan(&mid, &fid, &fwinner, &sid, &swinner)
			if err != nil {
				return models.TournamentBracket{}, err
			}
			sp.MatchID = mid
			if fwinner && fid.Valid {
				sp.WinnerID = fid.Int32
			} else if swinner && sid.Valid {
				sp.WinnerID = sid.Int32
			} else {
				continue
			}
			err = s.tryInitNewMatch(ctx, tx, key, fp, sp)
			if err != nil {
				return models.TournamentBracket{}, err
			}
		} else {
			err := s.tryInitNewMatch(ctx, tx, key, value[0], value[1])
			if err != nil {
				return models.TournamentBracket{}, err
			}
		}
	}

	if err := tx.Commit(ctx); err != nil {
		return models.TournamentBracket{}, err
	}

	updatedBracket, err := s.GetTournamentBracket(id)
	if err != nil {
		return models.TournamentBracket{}, err
	}

	return *updatedBracket, nil
}

func (s *TournamentService) tryInitNewMatch(ctx context.Context, tx pgx.Tx, next_match_id int32, fp, sp models.MatchResult) error {
	next_first_id := fp.WinnerID
	next_second_id := sp.WinnerID
	log.Println("Here")
	if sp.MatchID < fp.MatchID {
		next_first_id = sp.WinnerID
		next_second_id = fp.WinnerID
	}

	log.Println(fp, sp)

	_, err := tx.Exec(ctx, `
		UPDATE Match
		SET first_participant_id=$1, second_participant_id=$2
		WHERE id = $3
	`, next_first_id, next_second_id, next_match_id)

	return err
}

func (s *TournamentService) GetTeamPlayerConstraint(id int) (pgtype.Int4, pgtype.Int4, error) {
	ctx := context.Background()
	var min pgtype.Int4
	var max pgtype.Int4
	err := s.db.QueryRow(ctx, `
		SELECT min_team_limit, max_team_limit
		FROM Tournament
		WHERE id = $1;
	`, id).Scan(&min, &max)
	return min, max, err
}
