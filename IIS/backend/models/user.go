/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package models

import (
	"github.com/jackc/pgx/v5/pgtype"
)

type BaseUser struct {
	ID      int32       `json:"id"`
	Email   string      `json:"email"`
	Name    pgtype.Text `json:"name"`
	Surname pgtype.Text `json:"surname"`
}

type User struct {
	BaseUser
	Password string `json:"password,omitempty"`
	Role     string `json:"role"`
}

type RegisterUserRequest struct {
	Email    string `json:"email" binding:"required,email"`
	Password string `json:"password" binding:"required,alphanum"`
	Name     string `json:"name" binding:"required,alpha"`
	Surname  string `json:"surname" binding:"required,alpha"`
}

type RegisterUserResponse struct {
	ID      int32  `json:"id"`
	Email   string `json:"email"`
	Name    string `json:"name"`
	Surname string `json:"surname"`
	Role    string `json:"role"`
}

type LoginUserRequest struct {
	Email    string `json:"email" binding:"required,email"`
	Password string `json:"password" binding:"required,alphanum"`
}

type LoginUserResponse struct {
	ID      int32  `json:"id"`
	Email   string `json:"email"`
	Name    string `json:"name"`
	Surname string `json:"surname"`
	Role    string `json:"role"`
}

type AdminUpdateUserRequest struct {
	Email    string `json:"email" binding:"required,email"`
	Password string `json:"password" binding:"omitempty,alphanum"`
}

type UpdateUserRequest struct {
	Name    string `json:"name" binding:"required,alpha"`
	Surname string `json:"surname" binding:"required,alpha"`
	Email   string `json:"email" binding:"required,email"`
}

type Profile struct {
	ManagedTeams       []TeamAndPlayers            `json:"managing"`
	TeamInvites        []TeamPlayerInvite          `json:"team_invites"`
	CreatedTournaments []TournamentProfileDetailed `json:"created_tournaments"`
}
