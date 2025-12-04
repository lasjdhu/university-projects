/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package handlers

import (
	"backend/internal/errors"
	"backend/internal/validation"
	"backend/models"
	"backend/services"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
)

type TournamentHandler struct {
	tournamentService *services.TournamentService
}

func NewTournamentHandler(tournamentService *services.TournamentService) *TournamentHandler {
	return &TournamentHandler{tournamentService}
}

func (h *TournamentHandler) GetAdminTournaments(c *gin.Context) {
	if role, exists := c.Get("role"); !exists || role != "Admin" {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot access this resource"})
		return
	}

	page := c.Query("page")
	limit := c.Query("limit")
	state := c.Query("state")
	searchText := c.Query("search")

	pageInt, err := strconv.Atoi(page)
	if err != nil || pageInt < 1 {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter page was not specified"})
		return
	}
	limitInt, err := strconv.Atoi(limit)
	if err != nil || limitInt < 1 {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter limit was not specified"})
		return
	}

	detailed_tournaments, err := h.tournamentService.GetTournamentsDetailed(pageInt, limitInt, state, searchText)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, detailed_tournaments)
}

func (h *TournamentHandler) GetTournaments(c *gin.Context) {
	page := c.Query("page")
	limit := c.Query("limit")
	searchText := c.Query("search")
	team := c.Query("team")
	user := c.Query("user")

	pageInt, err := strconv.Atoi(page)
	if err != nil || pageInt < 1 {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter page was not specified"})
		return
	}
	limitInt, err := strconv.Atoi(limit)
	if err != nil || limitInt < 1 {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter limit was not specified"})
		return
	}

	teamInt := -1
	userInt := -1
	if team != "" {
		teamInt, err = strconv.Atoi(team)
		if err != nil || teamInt < 1 {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter team is inappropriate"})
			return
		}
	}
	if user != "" {
		userInt, err = strconv.Atoi(user)
		if err != nil || userInt < 1 {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter user is inappropriate"})
			return
		}
	}

	response, err := h.tournamentService.GetTournaments(pageInt, limitInt, teamInt, userInt, searchText)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, response)
}

func (h *TournamentHandler) GetTournamentById(c *gin.Context) {
	id := c.Param("id")
	tID, err := strconv.Atoi(id)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	tournament, err := h.tournamentService.GetTournamentById(int32(tID))
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, tournament)
}

func isPowerOfTwo(n int) bool {
	return n > 0 && (n&(n-1)) == 0
}

func (h *TournamentHandler) CreateTournament(c *gin.Context) {
	req := &models.CreateTournamentRequest{}
	if err := c.ShouldBindJSON(req); err != nil {
		code, msg := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(code, gin.H{"message": msg})
		return
	}
	if !isPowerOfTwo(int(req.ExpectedMembers)) {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "Capacity must be a number that is a power of 2."})
		return
	}

	if req.MinLimit != nil && req.MaxLimit != nil && *req.MaxLimit < *req.MinLimit {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid range for team player constraint"})
		return
	}

	managerID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "Missing identity"})
		return
	}

	tournament, err := h.tournamentService.CreateTournament(req, managerID.(int32))
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusCreated, tournament)
}

func (h *TournamentHandler) UpdateTournament(c *gin.Context) {
	idStr := c.Param("id")
	tID, err := strconv.Atoi(idStr)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid tournament ID"})
		return
	}

	req := &models.CreateTournamentRequest{}
	if err := c.ShouldBindJSON(req); err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": err.Error()})
		return
	}

	managerID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "Missing identity"})
		return
	}

	tournaments, err := h.tournamentService.GetTournamentsByManagerId(managerID.(int32))
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}

	isOwner := false
	for _, t := range tournaments {
		if t.ID == int32(tID) {
			isOwner = true
			break
		}
	}
	if !isOwner {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot update this tournament"})
		return
	}

	tournament, err := h.tournamentService.UpdateTournament(int32(tID), req)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}

	c.JSON(http.StatusOK, tournament)
}

func (h *TournamentHandler) DeleteTournament(c *gin.Context) {
	idStr := c.Param("id")

	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid tournament ID"})
		return
	}

	managerID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "Missing identity"})
		return
	}

	managerID, ok := managerID.(int32)
	if !ok {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid identity"})
		return
	}

	isManager, err := h.tournamentService.CanDelete(managerID.(int32), int32(id))
	if err != nil {
		c.Error(err)
		return
	}

	if !isManager {
		c.Error(errors.Wrap(nil, "You cannot delete this tournament", http.StatusForbidden))
		return
	}

	err = h.tournamentService.DeleteTournament(int32(id))
	if err != nil {
		c.Error(err)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Tournament deleted"})
}

func (h *TournamentHandler) GetTournamentBracket(c *gin.Context) {
	id := c.Param("id")

	bracket, err := h.tournamentService.GetTournamentBracket(id)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, bracket)
}

func (h *TournamentHandler) StartTournament(c *gin.Context) {
	id := c.Param("id")

	err := h.tournamentService.StartTournament(id)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Tournament started"})
}

func (h *TournamentHandler) UpdateTournamentState(c *gin.Context) {
	newStateRequest := models.TournamentStateRequest{}
	err := c.ShouldBindJSON(&newStateRequest)
	if err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	if role, exists := c.Get("role"); exists && role == "Admin" {
		err = h.tournamentService.UpdateTournamentState(newStateRequest.ID, newStateRequest.State)
		if err != nil {
			c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
			return
		}
		c.JSON(http.StatusNoContent, nil)
		return
	}

	managerID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "Missing identity"})
		return
	}

	tournaments, err := h.tournamentService.GetTournamentsByManagerId(managerID.(int32))
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	isOwner := false
	for _, t := range tournaments {
		if t.ID == newStateRequest.ID {
			isOwner = true
			break
		}
	}

	if !isOwner {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot change tournament state"})
		return
	}

	err = h.tournamentService.UpdateTournamentState(newStateRequest.ID, newStateRequest.State)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}

	c.JSON(http.StatusNoContent, nil)
}

func (h *TournamentHandler) UpdateTournamentBracket(c *gin.Context) {
	id := c.Param("id")

	req := &models.TournamentBracket{}
	if err := c.ShouldBindJSON(req); err != nil {
		code, msg := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(code, gin.H{"message": msg})
		return
	}
	manager, exists := c.Get("id")
	err := h.tournamentService.CheckTournamentBracket(id, req, manager.(int32), exists)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": err.Error()})
		return
	}
	updTournaments, err := h.tournamentService.UpdateTournamentBracket(id, req)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		return
	}
	c.JSON(http.StatusOK, updTournaments)
}
