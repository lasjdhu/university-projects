/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package models

import "github.com/jackc/pgx/v5/pgtype"

type Tournament struct {
	ID              int32  `json:"id"`
	Name            string `json:"name"`
	Discipline      string `json:"discipline"`
	ExpectedMembers int32  `json:"expected_members"`
	Type            string `json:"type"`
	ManagerID       int32  `json:"manager_id"`
	State           string `json:"state"`
}

type CreateTournamentRequest struct {
	Name            string `json:"name" binding:"required"`
	Discipline      string `json:"discipline" binding:"required"`
	ExpectedMembers int32  `json:"expected_members" binding:"required"`
	Type            string `json:"type" binding:"required"`
	Prize           int32  `json:"prize" binding:"min=0"`
	MinLimit        *int32 `json:"min_limit" binding:"omitempty,min=1"`
	MaxLimit        *int32 `json:"max_limit" binding:"omitempty,min=1"`
}

type MatchParticipant struct {
	ID         pgtype.Int4 `json:"id"`
	ResultText *string     `json:"result_text"`
	IsWinner   bool        `json:"is_winner"`
	Name       *string     `json:"name"` // nil => TBD
}

type BracketMatch struct {
	ID                  int64              `json:"id"`
	Name                string             `json:"name"`
	NextMatchID         pgtype.Int4        `json:"next_match_id"`
	TournamentRoundText string             `json:"tournament_round_text"`
	Date                pgtype.Timestamp   `json:"date"`
	Participants        []MatchParticipant `json:"participants"`
}

type TournamentBracket struct {
	Matches []BracketMatch `json:"matches"`
}

type TournamentBaseResponse struct {
	ID              int32  `json:"id"`
	Name            string `json:"name"`
	Discipline      string `json:"discipline"`
	ExpectedMembers int32  `json:"expected_members"`
	Type            string `json:"type"`
}

type TournamentAdminDetailed struct {
	TournamentBaseResponse
	Prize    int32  `json:"prize"`
	MinLimit int32  `json:"min_limit"`
	MaxLimit int32  `json:"max_limit"`
	State    string `json:"state"`
	Manager  Player `json:"manager"`
}

type TournamentProfileDetailed struct {
	TournamentBaseResponse
	Prize                 int32                                   `json:"prize"`
	MinLimit              int32                                   `json:"min_limit"`
	MaxLimit              int32                                   `json:"max_limit"`
	State                 string                                  `json:"state"`
	RequestedParticipants []TournamentParticipantConflictsMinimal `json:"participant_requests"`
}

type TournamentDetailed struct {
	TournamentBaseResponse
	Prize        int32                          `json:"prize"`
	MinLimit     int32                          `json:"min_limit"`
	MaxLimit     int32                          `json:"max_limit"`
	Participants []TournamentParticipantMinimal `json:"participants"`
	Manager      Player                         `json:"manager"`
}

type TournamentStateRequest struct {
	ID    int32  `json:"id"`
	State string `json:"state" binding:"required,oneof=Pending Accepted Rejected"`
}

type TournamentMinLimit struct {
	ID       int32
	MinLimit pgtype.Int4
}
