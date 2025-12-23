/**
 * IIS Project
 * @author Albert Tikaiev
 */
package models

type Overview struct {
	TournamentsCount int                      `json:"tournaments_count"`
	TeamsCount       int                      `json:"teams_count"`
	PlayersCount     int                      `json:"players_count"`
	Tournaments      []TournamentBaseResponse `json:"tournaments"`
	Teams            []Team                   `json:"teams"`
	Players          []Player                 `json:"players"`
}
