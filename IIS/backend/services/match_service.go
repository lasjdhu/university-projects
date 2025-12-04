/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package services

import (
	errori "backend/internal/errors"
	"backend/models"
	"context"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgxpool"
)

type MatchService struct {
	db *pgxpool.Pool
}

func NewMatchService(db *pgxpool.Pool) *MatchService {
	return &MatchService{db}
}

func (s *MatchService) CountPlayerMatches(userID int32, searchText string) (int, error) {
	ctx := context.Background()

	row := s.db.QueryRow(ctx, `WITH participated AS (
			SELECT tp.id, t.id AS tid, t.name AS tname FROM TournamentParticipant tp 
			JOIN Tournament t ON tp.tournament_id = t.id
			WHERE
      (tp.player_id IS NOT NULL AND tp.player_id = $1)
      OR
      (tp.team_id IS NOT NULL AND $1 IN (SELECT user_id FROM TeamPlayer p WHERE p.team_id = tp.team_id))
		)
		SELECT COUNT(*)
		FROM Match m
		JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
		JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
		LEFT JOIN "User" u1 ON tp1.player_id = u1.id
		LEFT JOIN "User" u2 ON tp2.player_id = u2.id
		LEFT JOIN Team t1 ON tp1.team_id = t1.id
		LEFT JOIN Team t2 ON tp2.team_id = t2.id
		JOIN participated p ON p.id = m.first_participant_id OR p.id = m.second_participant_id
		WHERE ($2 = ''
		OR similarity(COALESCE(t1.name, u1.name || ' ' ||u1.surname), $2) > 0.05
		OR similarity(COALESCE(t2.name, u2.name || ' ' ||u2.surname), $2) > 0.05)
		`, userID, searchText)

	var count int
	err := row.Scan(&count)
	return count, err
}

func (s *MatchService) GetMatchesProfilePlayer(userID, limit, offset int32, searchText string) ([]models.MatchDetailed, error) {
	ctx := context.Background()

	rows, err := s.db.Query(ctx, `WITH participated AS (
			SELECT tp.id, t.id AS tid, t.name AS tname, t.type as ttype FROM TournamentParticipant tp
			JOIN Tournament t ON tp.tournament_id = t.id
			WHERE
      (tp.player_id IS NOT NULL AND tp.player_id = $1)
      OR
      (tp.team_id IS NOT NULL AND $1 IN (SELECT user_id FROM TeamPlayer p WHERE p.team_id = tp.team_id))
		)
		SELECT m.id, m."date", p.ttype, p.tid, p.tname,
		COALESCE(t1.name, u1.name || ' ' ||u1.surname) as fname, COALESCE(t1.id, u1.id) as fid, first_participant_result_text, first_participant_is_winner,
		COALESCE(t2.name, u2.name || ' ' ||u2.surname) as sname, COALESCE(t2.id, u2.id) as sid, second_participant_result_text, second_participant_is_winner
		FROM Match m
		JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
		JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
		LEFT JOIN "User" u1 ON tp1.player_id = u1.id
		LEFT JOIN "User" u2 ON tp2.player_id = u2.id
		LEFT JOIN Team t1 ON tp1.team_id = t1.id
		LEFT JOIN Team t2 ON tp2.team_id = t2.id
		JOIN participated p ON p.id = m.first_participant_id OR p.id = m.second_participant_id
		WHERE ($2 = ''
		OR similarity(COALESCE(t1.name, u1.name || ' ' ||u1.surname), $2) > 0.05
		OR similarity(COALESCE(t2.name, u2.name || ' ' ||u2.surname), $2) > 0.05)
		ORDER BY m."date" DESC
		LIMIT $3 OFFSET $4
		`, userID, searchText, limit, offset)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}
	defer rows.Close()

	return scanMatches(rows)
}

func (s *MatchService) CountTeamMatches(teamID int32) (int, error) {
	ctx := context.Background()

	row := s.db.QueryRow(ctx, `
		SELECT COUNT(*)
		FROM Match m
		JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
		JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
		LEFT JOIN Team t1 ON tp1.team_id = t1.id
		LEFT JOIN Team t2 ON tp2.team_id = t2.id
		WHERE t1.id = $1 OR t2.id = $1
		`, teamID)

	var count int
	err := row.Scan(&count)
	return count, err
}

func (s *MatchService) GetMatchesProfileTeam(teamID, limit, offset int32) ([]models.MatchDetailed, error) {
	ctx := context.Background()

	rows, err := s.db.Query(ctx, `
		SELECT m.id, m."date", tour.type, tour.id, tour.name,
		t1.name, t1.id , first_participant_result_text, first_participant_is_winner,
		t2.name, t2.id, second_participant_result_text, second_participant_is_winner
		FROM Match m
		JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
		JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
		JOIN Tournament tour ON tp1.tournament_id = tour.id
		LEFT JOIN Team t1 ON tp1.team_id = t1.id
		LEFT JOIN Team t2 ON tp2.team_id = t2.id
		WHERE t1.id = $1 OR t2.id = $1
		ORDER BY m."date" DESC
		LIMIT $2 OFFSET $3
		`, teamID, limit, offset)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}
	defer rows.Close()

	return scanMatches(rows)
}

func (s *MatchService) CountMatches() (int, error) {
	ctx := context.Background()

	row := s.db.QueryRow(ctx, `
		SELECT COUNT(*)
		FROM Match m
	`)

	var count int
	err := row.Scan(&count)
	return count, err
}

func (s *MatchService) GetMatches(limit, offset int32) ([]models.MatchDetailed, error) {
	ctx := context.Background()
	rows, err := s.db.Query(ctx, `
		SELECT m.id, m."date", tour.type, tour.id, tour.name,
		COALESCE(t1.name, u1.name || ' ' ||u1.surname) as fname, COALESCE(t1.id, u1.id) as fid, first_participant_result_text, first_participant_is_winner,
		COALESCE(t2.name, u2.name || ' ' ||u2.surname) as sname, COALESCE(t2.id, u2.id) as sid, second_participant_result_text, second_participant_is_winner
		FROM Match m
		JOIN TournamentParticipant tp1 ON tp1.id = m.first_participant_id
		JOIN TournamentParticipant tp2 ON tp2.id = m.second_participant_id
		JOIN Tournament tour ON tour.id = tp1.tournament_id
		LEFT JOIN "User" u1 ON tp1.player_id = u1.id
		LEFT JOIN "User" u2 ON tp2.player_id = u2.id
		LEFT JOIN Team t1 ON tp1.team_id = t1.id
		LEFT JOIN Team t2 ON tp2.team_id = t2.id
		ORDER BY m."date" DESC
		LIMIT $1 OFFSET $2
		`, limit, offset)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, errori.DBNotFound
		}
		return nil, err
	}
	defer rows.Close()

	return scanMatches(rows)
}

func scanMatches(rows pgx.Rows) ([]models.MatchDetailed, error) {
	var matches []models.MatchDetailed

	for rows.Next() {
		var match models.MatchDetailed
		var first models.ResolvedMatchParticipant
		var second models.ResolvedMatchParticipant
		var fscore *string
		var sscore *string
		if err := rows.Scan(
			&match.ID,
			&match.Date,
			&match.MatchType,
			&match.TournamentID,
			&match.TournamentName,
			&first.Name,
			&first.ID,
			&fscore,
			&first.Winner,
			&second.Name,
			&second.ID,
			&sscore,
			&second.Winner); err != nil {
			return nil, err
		}
		if fscore != nil {
			first.Score = *fscore
		}
		if sscore != nil {
			second.Score = *sscore
		}
		match.First = first
		match.Second = second
		matches = append(matches, match)
	}

	return matches, nil
}
