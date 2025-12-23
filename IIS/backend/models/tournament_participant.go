/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package models

import "github.com/jackc/pgx/v5/pgtype"

type TournamentParticipant struct {
	ID           int32       `json:"id"`
	State        string      `json:"state"`
	TeamID       pgtype.Int4 `json:"team_id"`
	PlayerID     pgtype.Int4 `json:"player_id"`
	TournamentID int32       `json:"tournament_id"`
}

type TournamentParticipantMinimal struct {
	ID       int32       `json:"id"`
	PlayerID pgtype.Int4 `json:"user_id"`
	TeamID   pgtype.Int4 `json:"team_id"`
	Name     string      `json:"name"`
}

type TournamentParticipantConflictsMinimal struct {
	TournamentParticipantMinimal
	Conflicts []string `json:"conflicts"`
}

type CreateTournamentParticipant struct {
	TeamID   pgtype.Int4 `json:"team_id"`
	PlayerID pgtype.Int4 `json:"player_id"`
}

type ResolveTournamentParticipant struct {
	TournamentParticipantID int32  `json:"id" binding:"required"`
	Result                  string `json:"result" binding:"required,oneof=Accept Reject"`
}

type Player struct {
	ID      int32  `json:"id"`
	Name    string `json:"name"`
	Surname string `json:"surname"`
}

type PlayerDetail struct {
	ID          int32                 `json:"id"`
	Name        string                `json:"name"`
	Surname     string                `json:"surname"`
	Winnings    int32                 `json:"winnings"`
	Winrate     WinrateStatistic      `json:"winrate"`
	Disciplines []DisciplineStatistic `json:"disciplines"`
	Activity    []ActivityStatistic   `json:"activity"`
}
