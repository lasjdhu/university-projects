/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package models

import "github.com/jackc/pgx/v5/pgtype"

type Stage struct {
	ID           int32       `json:"id"`
	TournamentID int32       `json:"tournament_id"`
	Level        pgtype.Int4 `json:"level"`
}
