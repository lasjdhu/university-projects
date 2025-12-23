/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package handlers

import (
	"backend/models"
	"backend/services"
	"fmt"
	"net/http"

	"github.com/gin-gonic/gin"
)

type OverviewHandler struct {
	tournamentParticipantService *services.TournamentParticipantService
	tournamentService            *services.TournamentService
	teamService                  *services.TeamService
	s3Service                    *services.S3Service
}

func NewOverviewHandler(
	tourparts *services.TournamentParticipantService,
	tours *services.TournamentService,
	teams *services.TeamService,
	s3 *services.S3Service,
) *OverviewHandler {
	return &OverviewHandler{tourparts, tours, teams, s3}
}

func (h *OverviewHandler) GetOverview(c *gin.Context) {
	overview := models.Overview{}

	tournaments, err := h.tournamentService.GetTournaments(1, 4, -1, -1, "")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "internal error"})
		return
	}

	teams, err := h.teamService.GetTeams(1, 5, -1, "")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "internal error"})
		return
	}

	for i := range teams.Data {
		url, err := h.s3Service.GetPresignURL(fmt.Sprintf("team%d", teams.Data[i].ID))
		if err == nil {
			teams.Data[i].Image = url
		}
	}

	players, err := h.tournamentParticipantService.GetPlayers(1, 5, "")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "internal error"})
		return
	}

	overview.TournamentsCount = tournaments.TotalRecords
	overview.TeamsCount = teams.TotalRecords
	overview.PlayersCount = players.TotalRecords
	overview.Tournaments = tournaments.Data
	overview.Teams = teams.Data
	overview.Players = players.Data

	c.JSON(http.StatusOK, overview)
}
