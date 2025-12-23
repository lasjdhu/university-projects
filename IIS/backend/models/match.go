/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package models

import (
	"github.com/jackc/pgx/v5/pgtype"
)

type Match struct {
	ID                int32            `json:"id"`
	StageID           int32            `json:"stage_id"`
	NextMatchID       pgtype.Int4      `json:"next_match_id"`
	Name              string           `json:"name"`
	FirstParticipant  pgtype.Int4      `json:"first_participant"`
	SecondParticipant pgtype.Int4      `json:"second_participant"`
	Start             pgtype.Timestamp `json:"start"`
}

type ResolvedMatchParticipant struct {
	ID     int32  `json:"id"`
	Name   string `json:"name"`
	Winner bool   `json:"is_winner"`
	Score  string `json:"score"`
}

type MatchDetailed struct {
	ID             int32                    `json:"id"`
	Date           pgtype.Timestamp         `json:"date"`
	MatchType      string                   `json:"type"`
	TournamentName string                   `json:"tournament_name"`
	TournamentID   int32                    `json:"tournament_id"`
	First          ResolvedMatchParticipant `json:"first"`
	Second         ResolvedMatchParticipant `json:"second"`
}

type MatchResult struct {
	MatchID  int32
	WinnerID int32
}
