/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package handlers

import (
	errori "backend/internal/errors"
	"backend/internal/validation"
	"backend/models"
	"backend/services"
	"log"
	"net/http"
	"strconv"
	"time"

	"github.com/gin-gonic/gin"
)

type TournamentParticipantHandler struct {
	tournamentParticipantService *services.TournamentParticipantService
	tournamentService            *services.TournamentService
	teamService                  *services.TeamService
}

func NewTournamentParticipantHandler(
	tournamentParticipantService *services.TournamentParticipantService,
	tournamentService *services.TournamentService,
	teamService *services.TeamService) *TournamentParticipantHandler {
	return &TournamentParticipantHandler{tournamentParticipantService, tournamentService, teamService}
}

func (h *TournamentParticipantHandler) GetAllTournamentParticipants(c *gin.Context) {
	tournament_participants, err := h.tournamentParticipantService.GetAllTournamentParticipants()
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, tournament_participants)
}

func (h *TournamentParticipantHandler) GetPlayers(c *gin.Context) {
	page := c.Query("page")
	limit := c.Query("limit")
	searchText := c.Query("search")

	pageInt, err := strconv.Atoi(page)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter page was not specified"})
		return
	}
	limitInt, err := strconv.Atoi(limit)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter limit was not specified"})
		return
	}

	response, err := h.tournamentParticipantService.GetPlayers(pageInt, limitInt, searchText)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}
	c.JSON(http.StatusOK, response)
}

func (h *TournamentParticipantHandler) GetPlayerById(c *gin.Context) {
	idStr := c.Param("id")
	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.JSON(http.StatusBadRequest, gin.H{"message": "Invalid player ID"})
		return
	}

	player, err := h.tournamentParticipantService.GetPlayerById(id)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	if player.ID == 0 {
		c.JSON(http.StatusNotFound, gin.H{"message": "Player not found"})
		return
	}

	player.Winrate, err = h.tournamentParticipantService.GetPlayerWinrate(player.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	if player.Winrate.Loses == 0 {
		if player.Winrate.Wins == 0 {
			player.Winrate.Percentage = 0
		} else {
			player.Winrate.Percentage = 100
		}
	} else {
		player.Winrate.Percentage = int((float64(player.Winrate.Wins) / (float64(player.Winrate.Wins) + float64(player.Winrate.Loses))) * 100)
	}

	player.Disciplines, err = h.tournamentParticipantService.GetPlayerDisciplines(player.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	matchmap, err := h.tournamentParticipantService.GetPlayerActivity(player.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	for i := time.Now().AddDate(0, -3, 0); i.Month() <= time.Now().Month(); i = i.AddDate(0, 1, 0) {
		if monthstat, ok := matchmap[i.Month().String()]; ok {
			player.Activity = append(player.Activity, monthstat)
		} else {
			player.Activity = append(player.Activity, models.ActivityStatistic{Month: i.Month().String(), Personal: 0, Teams: 0})
		}
	}

	player.Winnings, err = h.tournamentParticipantService.GetPlayerWinnings(player.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	c.JSON(http.StatusOK, player)
}

func (h *TournamentParticipantHandler) CreateParticipant(c *gin.Context) {
	id := c.Param("id")
	tID, err := strconv.Atoi(id)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}
	authid, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "You are unauthorized"})
		return
	}

	tournament, err := h.tournamentService.GetTournamentById(int32(tID))
	if tournament.Participants != nil && len(tournament.Participants) >= int(tournament.ExpectedMembers) {
		c.AbortWithStatusJSON(http.StatusNotAcceptable, gin.H{"message": "Tournament capacity is already full"})
		return
	}
	req := &models.CreateTournamentParticipant{}
	if err := c.ShouldBindJSON(req); err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	switch tournament.Type {
	case "Team":
		if !req.TeamID.Valid {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid team ID"})
			return
		}

		_, managerID, err := h.teamService.GetTeamById(int(req.TeamID.Int32))

		if err != nil {
			if err == errori.DBNotFound {
				c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Such team does not exist"})
			} else {
				c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Cannot find team"})
			}
			return
		}

		if managerID != authid {
			c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot invite this team"})
			return
		}

		teamPlayers := h.teamService.CountActiveTeamPlayers(req.TeamID.Int32)
		min_limit, max_limit, err := h.tournamentService.GetTeamPlayerConstraint(tID)
		log.Println(teamPlayers, err)
		if err != nil || teamPlayers == -1 {
			c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Cannot check tournament constraints"})
			return
		} else if min_limit.Valid {
			if min_limit.Int32 > teamPlayers {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "There are not enough players on the team for this tournament"})
				return
			}
		} else if max_limit.Valid {
			if max_limit.Int32 < teamPlayers {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "There are many players in the team for this tournament"})
				return
			}
		}

		_, err = h.tournamentParticipantService.TeamOrPlayerPaticipatesTournament(int32(tID), req.TeamID.Int32)
		if err != errori.DBNotFound {
			if err != nil {
				c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Cannot find participants for this tournament"})
				return
			} else {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Cannot add a team that or a player that is already participates the tournament"})
				return
			}
		}
	case "Person":
		if !req.PlayerID.Valid {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid player ID"})
			return
		}
		if authid != req.PlayerID.Int32 {
			c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot somebody else to the tournament"})
			return
		}
		_, err = h.tournamentParticipantService.PlayerPaticipatesTournament(int32(tID), req.PlayerID.Int32)
		if err != errori.DBNotFound {
			if err != nil {
				c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Cannot find participants for this tournament"})
				return
			} else {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Cannot add a player that is already participates the tournament"})
				return
			}
		}
	}

	updTournaments, err := h.tournamentParticipantService.CreateTournamentParticipant(id, req, tournament.Type)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}
	c.JSON(http.StatusOK, updTournaments)
}

func (h *TournamentParticipantHandler) ResolveParticipant(c *gin.Context) {
	id := c.Param("id")
	tID, err := strconv.Atoi(id)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	tournament, err := h.tournamentService.GetTournamentById(int32(tID))
	req := &models.ResolveTournamentParticipant{}
	if err := c.ShouldBindJSON(req); err != nil {
		code, msg := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(code, gin.H{"message": msg})
		return
	}
	if len(tournament.Participants) >= int(tournament.ExpectedMembers) && req.Result == "Accept" {
		c.AbortWithStatusJSON(http.StatusNotAcceptable, gin.H{"message": "Tournament capacity is already full"})
		return
	}

	if authid, exists := c.Get("id"); !exists || authid != tournament.Manager.ID {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You do not have right to accept or reject players"})
		return
	}

	err = h.tournamentParticipantService.ResolveTournamentParticipant(req.TournamentParticipantID, req.Result+"ed")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}
	c.JSON(http.StatusOK, gin.H{"message": "Success"})
}
