/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package models

import (
	"github.com/jackc/pgx/v5/pgtype"
)

type Team struct {
	ID          int32       `json:"id"`
	Name        string      `json:"name"`
	Description string      `json:"description"`
	Image       string      `json:"image"`
	Since       pgtype.Date `json:"since"`
	ManagerID   int32       `json:"manager_id"`
}

type TeamBaseResponse struct {
	ID          int32       `json:"id"`
	Name        string      `json:"name"`
	Description string      `json:"description"`
	Image       string      `json:"image"`
	Since       pgtype.Date `json:"since"`
}

type TeamAndPlayers struct {
	TeamBaseResponse
	ConfilctTournaments []string         `json:"confilct_tournaments"`
	Players             []TeamUserPlayer `json:"players"`
}

type TeamDetail struct {
	TeamBaseResponse
	Manager     Player                `json:"manager"`
	Winnings    int32                 `json:"winnings"`
	Players     []Player              `json:"players"`
	Winrate     WinrateStatistic      `json:"winrate"`
	Disciplines []DisciplineStatistic `json:"disciplines"`
	Activity    []ActivityStatistic   `json:"activity"`
}

type CreateTeamRequest struct {
	Name         string   `json:"name" binding:"required,min=3,max=30"`
	Description  string   `json:"description" binding:"max=300"`
	EmailInvites []string `json:"email_invites" binding:"dive,email"`
}

type UpdateTeamRequest struct {
	Name        string `json:"name" binding:"required,min=3,max=30"`
	Description string `json:"description" binding:"max=300"`
}
