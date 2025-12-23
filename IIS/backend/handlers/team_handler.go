/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package handlers

import (
	"backend/internal/errors"
	"backend/internal/validation"
	"backend/models"
	"backend/services"
	"context"
	"fmt"
	"net/http"
	"strconv"
	"strings"
	"time"

	"github.com/gin-gonic/gin"
)

type TeamHandler struct {
	teamService       *services.TeamService
	teamPlayerService *services.TeamPlayerService
	userService       *services.UserService
	s3Service         *services.S3Service
}

func NewTeamHandler(
	teamService *services.TeamService,
	s3Service *services.S3Service,
	teamPlayerService *services.TeamPlayerService,
	userService *services.UserService,
) *TeamHandler {
	return &TeamHandler{teamService, teamPlayerService, userService, s3Service}
}

func (h *TeamHandler) GetAllTeams(c *gin.Context) {
	teams, err := h.teamService.GetAllTeams()
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, teams)
}

func (h *TeamHandler) GetTeams(c *gin.Context) {
	page := c.Query("page")
	limit := c.Query("limit")
	searchText := c.Query("search")
	user := c.Query("user")

	pageInt, err := strconv.Atoi(page)
	if err != nil {
		c.Error(errors.Wrap(err, "Page was not specified", http.StatusBadRequest))
		return
	}
	limitInt, err := strconv.Atoi(limit)
	if err != nil {
		c.Error(errors.Wrap(err, "Limit was not specified", http.StatusBadRequest))
		return
	}

	userInt := -1
	if user != "" {
		userInt, err = strconv.Atoi(user)
		if err != nil || userInt < 1 {
			c.Error(errors.Wrap(err, "User is inappropriate", http.StatusBadRequest))
			return
		}
	}

	response, err := h.teamService.GetTeams(pageInt, limitInt, userInt, searchText)
	if err != nil {
		c.Error(err)
		return
	}

	for i := range response.Data {
		url, err := h.s3Service.GetPresignURL(fmt.Sprintf("team%d", response.Data[i].ID))
		if err == nil {
			response.Data[i].Image = url
		}
	}

	c.JSON(http.StatusOK, response)
}

func (h *TeamHandler) GetTeamById(c *gin.Context) {
	idStr := c.Param("id")
	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid team ID", http.StatusBadRequest))
		return
	}

	team, err := h.teamService.GetTeamDetailById(id)
	if err != nil {
		c.Error(err)
		return
	}

	url, err := h.s3Service.GetPresignURL(fmt.Sprintf("team%d", team.ID))
	if err == nil {
		team.Image = url
	}

	team.Winrate, err = h.teamService.GetTeamWinrate(team.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	if team.Winrate.Loses == 0 {
		if team.Winrate.Wins == 0 {
			team.Winrate.Percentage = 0
		} else {
			team.Winrate.Percentage = 100
		}
	} else {
		team.Winrate.Percentage = int((float64(team.Winrate.Wins) / (float64(team.Winrate.Wins) + float64(team.Winrate.Loses))) * 100)
	}

	team.Disciplines, err = h.teamService.GetTeamDisciplines(team.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	matchmap, err := h.teamService.GetTeamActivity(team.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	for i := time.Now().AddDate(0, -3, 0); i.Month() <= time.Now().Month(); i = i.AddDate(0, 1, 0) {
		if monthstat, ok := matchmap[i.Month().String()]; ok {
			team.Activity = append(team.Activity, monthstat)
		} else {
			team.Activity = append(team.Activity, models.ActivityStatistic{Month: i.Month().String(), Personal: 0, Teams: 0})
		}
	}

	team.Winnings, err = h.teamService.GetTeamWinnings(team.ID)
	if err != nil {
		c.AbortWithError(http.StatusInternalServerError, err)
		return
	}

	c.JSON(http.StatusOK, team)
}

func (h *TeamHandler) UpdateTeamAvatar(c *gin.Context) {
	idStr := c.Param("id")

	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid team ID", http.StatusBadRequest))
		return
	}

	team, err := h.teamService.GetTeamDetailById(id)
	if err != nil {
		c.Error(err)
		return
	}

	if authid, exists := c.Get("id"); !exists || authid != team.Manager.ID {
		c.Error(errors.Wrap(nil, "You do not have right to change avatar", http.StatusForbidden))
		return
	}

	contentTypeSplitted := strings.Split(c.ContentType(), "/")
	if contentTypeSplitted[0] != "image" || len(contentTypeSplitted) != 2 {
		c.Error(errors.Wrap(nil, "Invalid content type", http.StatusBadRequest))
		return
	}

	if contentTypeSplitted[1] != "jpeg" &&
		contentTypeSplitted[1] != "jpg" &&
		contentTypeSplitted[1] != "gif" &&
		contentTypeSplitted[1] != "png" {
		c.Error(errors.Wrap(nil, "Invalid image type", http.StatusBadRequest))
		return
	}

	err = h.s3Service.DeleteObject(fmt.Sprintf("team%d", id))

	if err != nil {
		c.Error(err)
		return
	}

	err = h.s3Service.PutObject(
		fmt.Sprintf("avatars/team%d.%s", id, contentTypeSplitted[1]),
		c.ContentType(),
		c.Request.ContentLength,
		c.Request.Body,
	)

	if err != nil {
		c.Error(err)
		return
	}

	c.JSON(http.StatusOK, gin.H{})
}

func (h *TeamHandler) ChangePlayerState(c *gin.Context) {
	tIdStr := c.Param("id")
	pIdStr := c.Param("pid")
	tId, err := strconv.Atoi(tIdStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid team ID", http.StatusBadRequest))
		return
	}

	pId, err := strconv.Atoi(pIdStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid player ID", http.StatusBadRequest))
		return
	}

	var req models.TeamPlayerStateRequest
	var resp models.TeamAndPlayers
	if err := c.ShouldBindJSON(&req); err != nil {
		c.Error(errors.Wrap(err, "Bad request", http.StatusBadRequest))
		return
	}

	team, managerID, err := h.teamService.GetTeamById(tId)
	if err != nil {
		c.Error(err)
		return
	}

	if authid, exists := c.Get("id"); !exists || authid != managerID {
		c.Error(errors.Wrap(nil, "You do not have right to player state", http.StatusBadRequest))
		return
	}
	resp.TeamBaseResponse = team

	player, err := h.teamPlayerService.GetPlayer(int32(pId))
	if err != nil {
		c.Error(err)
		return
	}
	if player.TeamID != int32(tId) {
		c.Error(errors.Wrap(nil, "Player was not playing in this team", http.StatusBadRequest))
		return
	}

	if player.State != req.CurrentState {
		c.Error(errors.Wrap(nil, "You have outdated state", http.StatusPreconditionFailed))
		return
	}

	switch req.CurrentState {
	case "Active":
		err = h.teamPlayerService.Delete(int32(pId))
	case "Invited":
		err = h.teamPlayerService.Cancel(int32(pId))
	}

	if err != nil {
		if err == errors.NotAcceptable {
			c.Error(errors.Wrap(nil, "Cannot delete team player since your team participating in some tournament", http.StatusBadRequest))
		} else {
			c.Error(err)
		}
		return
	}

	url, err := h.s3Service.GetPresignURL(fmt.Sprintf("team%d", team.ID))
	if err == nil {
		resp.Image = url
	}

	resp.Players, err = h.teamPlayerService.PlayersFromTeam(int32(tId))
	if err != nil {
		c.Error(err)
		return
	}

	c.JSON(http.StatusOK, resp)
}

func (h *TeamHandler) InvitePlayer(c *gin.Context) {
	var req models.TeamPlayerInviteRequest
	var resp models.TeamUserPlayer

	idStr := c.Param("id")
	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid team ID"})
		return
	}

	if err := c.ShouldBindJSON(&req); err != nil {
		c.Error(errors.Wrap(err, "Bad request", http.StatusBadRequest))
		return
	}

	_, managerID, err := h.teamService.GetTeamById(id)
	if err != nil {
		c.Error(err)
		return
	}

	if authid, exists := c.Get("id"); !exists || authid != managerID {
		c.Error(errors.Wrap(nil, "You do not have right to player state", http.StatusForbidden))
		return
	}

	user, err := h.userService.GetUserByEmail(context.Background(), req.Email)
	if err != nil {
		c.Error(err)
		return
	}
	if user == nil || user.Role == "Admin" {
		c.Error(errors.Wrap(nil, "User with such email does not exist", http.StatusNotFound))
		return
	}
	if user.ID == managerID {
		c.Error(errors.Wrap(nil, "You cannot invite yourself to the team", http.StatusBadRequest))
		return
	}

	_, _, err = h.teamPlayerService.GetActivePlayer(int32(user.ID), int32(id))
	if err != errors.DBNotFound && err == nil {
		c.Error(errors.Wrap(nil, "User is already in the team", http.StatusBadRequest))
		return
	}
	if err != errors.DBNotFound && err != nil {
		c.Error(err)
		return
	}

	player, err := h.teamPlayerService.AddInvitedPlayer(user.ID, int32(id))
	if err != nil {
		c.Error(err)
		return
	}

	resp.PlayerID = player.ID
	resp.UserID = user.ID
	resp.Name = user.Name.String
	resp.Surname = user.Surname.String
	resp.State = player.State

	c.JSON(http.StatusOK, resp)
}

func (h *TeamHandler) ResolveInvite(c *gin.Context) {
	var req models.ResolveInviteRequest

	tIdStr := c.Param("id")
	tId, err := strconv.Atoi(tIdStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid team ID", http.StatusBadRequest))
		return
	}

	if err := c.ShouldBindJSON(&req); err != nil {
		c.Error(errors.Wrap(err, "Bad request", http.StatusBadRequest))
		return
	}

	authid, exists := c.Get("id")
	if !exists {
		c.Error(errors.Wrap(err, "Unauthorized", http.StatusUnauthorized))
		return
	}

	if state, err := h.teamPlayerService.GetTeamPlayerState(authid.(int32), int32(tId)); err != nil || state != "Invited" {
		c.Error(errors.Wrap(err, "You are in the team already or your invite expired", http.StatusBadRequest))
		return
	}

	switch req.Result {
	case "Reject":
		err := h.teamPlayerService.RejectInvite(authid.(int32), int32(tId))
		if err != nil {
			c.Error(err)
			return
		}
	case "Accept":
		err := h.teamPlayerService.AcceptInvite(authid.(int32), int32(tId))
		if err != nil {
			c.Error(err)
			return
		}
	}

	c.JSON(http.StatusCreated, gin.H{"message": "Invite was resolved successfully"})
}

func (h *TeamHandler) CreateTeam(c *gin.Context) {
	var req models.CreateTeamRequest

	if err := c.ShouldBindJSON(&req); err != nil {
		c.Error(errors.Wrap(err, "Failed to bind JSON", http.StatusBadRequest))
		return
	}

	authid, exists := c.Get("id")
	if !exists {
		c.Error(errors.Wrap(nil, "Missing authentication ID", http.StatusUnauthorized))
		return
	}

	team, err := h.teamService.CreateTeamWithInvites(req.Name, req.Description, authid.(int32), req.EmailInvites)
	if err != nil {
		c.Error(err)
		return
	}

	c.JSON(http.StatusCreated, team)
}

func (h *TeamHandler) UpdateTeam(c *gin.Context) {
	idStr := c.Param("id")
	id, err := strconv.Atoi(idStr)
	if err != nil {
		c.Error(errors.Wrap(err, "Invalid team ID", http.StatusBadRequest))
		c.JSON(http.StatusBadRequest, gin.H{"message": "Invalid team ID"})
		return
	}

	var req models.UpdateTeamRequest

	if err := c.ShouldBindJSON(&req); err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	authid, exists := c.Get("id")
	if !exists {
		c.Error(errors.Wrap(nil, "Missing authentication ID", http.StatusUnauthorized))
		return
	}

	_, managerID, err := h.teamService.GetTeamById(id)
	if err != nil {
		c.Error(err)
		return
	}
	if managerID != authid {
		c.Error(errors.Wrap(nil, "You do not have right to update team infomation", http.StatusForbidden))
		return
	}

	err = h.teamService.UpdateTeam(int32(id), req.Name, req.Description)
	if err != nil {
		c.Error(err)
		return
	}

	c.JSON(http.StatusOK, gin.H{"message": "Team was successfully updated"})
}
