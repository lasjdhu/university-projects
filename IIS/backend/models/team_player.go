/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package models

import (
	"github.com/jackc/pgx/v5/pgtype"
)

type TeamPlayer struct {
	ID     int32            `json:"id"`
	UserID int32            `json:"user_id"`
	TeamID int32            `json:"team_id"`
	Since  pgtype.Timestamp `json:"since"`
	Until  pgtype.Timestamp `json:"until"`
	State  string           `json:"state"`
}

type TeamUserPlayer struct {
	PlayerID int32  `json:"player_id"`
	UserID   int32  `json:"id"`
	Name     string `json:"name"`
	Surname  string `json:"surname"`
	State    string `json:"state"`
}

type TeamPlayerInvite struct {
	PlayerID       int32  `json:"player_id"`
	TeamID         int32  `json:"team_id"`
	TeamName       string `json:"team_name"`
	ManagerID      int32  `json:"manager_id"`
	ManagerName    string `json:"manager_name"`
	ManagerSurname string `json:"manager_surname"`
}

type TeamPlayerInviteRequest struct {
	Email string `json:"email" binding:"required,email"`
}

type ResolveInviteRequest struct {
	PlayerID int32  `json:"player_id"`
	Result   string `json:"result" binding:"required,oneof=Accept Reject"`
}

type TeamPlayerStateRequest struct {
	CurrentState string `json:"state" binding:"required,oneof=Active Inactive Invited"`
}
