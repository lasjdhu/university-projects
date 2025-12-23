/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package services

import (
	errori "backend/internal/errors"
	"backend/models"
	"context"
	"time"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgtype"
	"github.com/jackc/pgx/v5/pgxpool"
)

type TeamPlayerService struct {
	db *pgxpool.Pool
}

func NewTeamPlayerService(db *pgxpool.Pool) *TeamPlayerService {
	return &TeamPlayerService{db}
}

func (s *TeamPlayerService) AcceptInvite(userID, teamID int32) error {
	ctx := context.Background()
	_, err := s.db.Exec(ctx, `
		UPDATE TeamPlayer
		SET since = CURRENT_DATE, state = 'Active'
		WHERE user_id = $1 AND team_id = $2 AND since IS NULL
	`, userID, teamID)
	return err
}

func (s *TeamPlayerService) RejectInvite(userID, teamID int32) error {
	ctx := context.Background()
	_, err := s.db.Exec(ctx, `
		DELETE FROM TeamPlayer
		WHERE user_id = $1 AND team_id = $2 AND since IS NULL
	`, userID, teamID)
	return err
}

func (s *TeamPlayerService) Delete(playerID int32) error {
	ctx := context.Background()
	tx, err := s.db.Begin(ctx)
	if err != nil {
		return err
	}
	defer tx.Rollback(ctx)

	var count int
	err = tx.QueryRow(ctx, `
	WITH init AS (
		SELECT team_id AS id FROM TeamPlayer WHERE id = $1
	)
	SELECT COUNT(*) FROM TeamPlayer tp
	JOIN init i ON i.id = tp.team_id
	WHERE tp.since IS NOT NULL AND tp.until IS NULL
	`, playerID).Scan(&count)
	if err != nil {
		return err
	}
	count = count - 1

	_, err = tx.Exec(ctx, `
		UPDATE TeamPlayer
		SET until = CURRENT_DATE, state = 'Inactive'
		WHERE id= $1
	`, playerID)
	if err != nil {
		return err
	}

	tourrows, err := tx.Query(ctx, `
	SELECT tour.id, tour.min_team_limit FROM TeamPlayer tp
	JOIN Team t ON t.id=tp.team_id
	JOIN TournamentParticipant tourp ON tourp.team_id = t.id
	JOIN Tournament tour ON tourp.tournament_id = tour.id
	WHERE tp.id = $1
	`, playerID)
	if err != nil {
		return err
	}
	defer tourrows.Close()

	var limits []models.TournamentMinLimit
	for tourrows.Next() {
		var limit models.TournamentMinLimit
		if err := tourrows.Scan(&limit.ID, &limit.MinLimit); err != nil {
			return err
		}
		limits = append(limits, limit)
	}

	for _, l := range limits {
		if l.MinLimit.Valid && count < int(l.MinLimit.Int32) {
			var not_started bool
			if err := tx.QueryRow(ctx, `SELECT NOT EXISTS(
				SELECT * FROM Stage WHERE tournament_id = $1
				)
				`, l.ID).Scan(&not_started); err != nil {
				return err
			}
			if not_started {
				_, err := tx.Exec(ctx, `
				DELETE FROM TournamentParticipant tp
				WHERE tp.tournament_id = $1 AND tp.state = 'Accepted'
				AND tp.team_id = (SELECT teamp.team_id FROM TeamPlayer teamp WHERE teamp.id = $2)`,
					l.ID, playerID)
				if err != nil {
					return err
				}
			} else {
				var tour_ended bool
				if err := tx.QueryRow(ctx, `SELECT EXISTS(
				SELECT * FROM Match m
				JOIN Stage s ON s.id = m.stage_id
				WHERE s.tournament_id = $1 AND s.level = (SELECT MAX(level) FROM Stage s2 WHERE s2.tournament_id =$1)
				AND (m.first_participant_is_winner OR m.second_participant_is_winner)
				)
				`, l.ID).Scan(&tour_ended); err != nil {
					return err
				}
				if !tour_ended {
					return errori.NotAcceptable
				}
			}
		}
	}

	if err = tx.Commit(ctx); err != nil {
		return err
	}

	return nil
}

func (s *TeamPlayerService) Cancel(playerID int32) error {
	ctx := context.Background()
	_, err := s.db.Exec(ctx, `
		DELETE FROM TeamPlayer
		WHERE id = $1
	`, playerID)
	return err
}

func (s *TeamPlayerService) PlayersFromTeam(teamID int32) ([]models.TeamUserPlayer, error) {
	ctx := context.Background()
	var players []models.TeamUserPlayer

	playerRows, err := s.db.Query(ctx, `
		SELECT tp.id, u.id, u.name, u.surname, tp.state
		FROM TeamPlayer tp
		JOIN "User" u ON u.id = tp.user_id
		WHERE tp.team_id = $1
		AND tp.until IS NULL
	`, teamID)
	if err != nil {
		return players, err
	}
	defer playerRows.Close()

	for playerRows.Next() {
		var player models.TeamUserPlayer
		err = playerRows.Scan(
			&player.PlayerID,
			&player.UserID,
			&player.Name,
			&player.Surname,
			&player.State,
		)
		if err != nil {
			return players, err
		}
		players = append(players, player)
	}

	return players, nil
}

func (s *TeamPlayerService) GetPlayer(playerID int32) (models.TeamPlayer, error) {
	ctx := context.Background()
	var player models.TeamPlayer
	row := s.db.QueryRow(ctx, `
		SELECT team_id, since, until, state FROM TeamPlayer
		WHERE id = $1
	`, playerID)

	err := row.Scan(
		&player.TeamID,
		&player.Since,
		&player.Until,
		&player.State,
	)
	if err != nil {
		return player, err
	}

	return player, nil
}

func (s *TeamPlayerService) GetActivePlayer(userID, teamID int32) (time.Time, models.BaseUser, error) {
	ctx := context.Background()

	var player models.BaseUser
	var since pgtype.Date

	row := s.db.QueryRow(ctx, `
		SELECT u.id, u.email, u.name, u.surname, tp.since FROM TeamPlayer tp
		JOIN "User" u ON u.id = tp.user_id
		WHERE tp.user_id = $1 AND tp.team_id = $2 AND tp.since IS NOT NULL AND tp.until IS NULL
	`, userID, teamID)

	err := row.Scan(
		&player.ID,
		&player.Email,
		&player.Name,
		&player.Surname,
		&since,
	)
	if err == pgx.ErrNoRows {
		return since.Time, player, errori.DBNotFound
	}
	if err != nil {
		return since.Time, player, err
	}

	return since.Time, player, nil
}

func (s *TeamPlayerService) AddInvitedPlayer(userID, teamID int32) (models.TeamPlayer, error) {
	ctx := context.Background()
	var player models.TeamPlayer

	row := s.db.QueryRow(ctx, `
		INSERT INTO TeamPlayer(user_id, team_id) VALUES($1, $2)
		RETURNING *
	`, userID, teamID)

	err := row.Scan(
		&player.ID,
		&player.UserID,
		&player.TeamID,
		&player.Since,
		&player.Until,
		&player.State,
	)
	if err != nil {
		return player, err
	}

	return player, nil
}

func (s *TeamPlayerService) GetInvitations(userID int32) ([]models.TeamPlayerInvite, error) {
	ctx := context.Background()
	var invites []models.TeamPlayerInvite

	rows, err := s.db.Query(ctx, `
		SELECT tp.id, tp.team_id, t.name, u.id, u.name, u.surname FROM TeamPlayer tp
		JOIN Team t ON tp.team_id = t.id
		JOIN "User" u ON t.manager_id = u.id
		WHERE tp.user_id = $1 AND tp.state = 'Invited'
	`, userID)
	if err != nil {
		return nil, err
	}
	defer rows.Close()

	for rows.Next() {
		var invite models.TeamPlayerInvite
		err = rows.Scan(
			&invite.PlayerID,
			&invite.TeamID,
			&invite.TeamName,
			&invite.ManagerID,
			&invite.ManagerName,
			&invite.ManagerSurname,
		)
		if err != nil {
			return nil, err
		}
		invites = append(invites, invite)
	}

	return invites, nil
}

func (s *TeamPlayerService) GetTeamPlayerState(userID, teamID int32) (string, error) {
	ctx := context.Background()
	row := s.db.QueryRow(ctx, `
	 SELECT state FROM TeamPlayer
	 WHERE user_id = $1 AND team_id = $2
	`, userID, teamID)
	var state string
	err := row.Scan(&state)
	return state, err
}
