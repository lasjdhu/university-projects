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
	"context"
	"fmt"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
	"golang.org/x/crypto/bcrypt"
)

type UserHandler struct {
	userService       *services.UserService
	matchService      *services.MatchService
	teamService       *services.TeamService
	tournamentService *services.TournamentService
	teamPlayerService *services.TeamPlayerService
	s3Service         *services.S3Service
}

func NewUserHandler(
	userService *services.UserService,
	matchService *services.MatchService,
	teamService *services.TeamService,
	tournamentService *services.TournamentService,
	teamPlayerService *services.TeamPlayerService,
	s3Service *services.S3Service) *UserHandler {
	return &UserHandler{userService, matchService, teamService, tournamentService, teamPlayerService, s3Service}
}

func (h *UserHandler) GetAllUsers(c *gin.Context) {
	if role, exists := c.Get("role"); !exists || role != "Admin" {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot access this resource"})
		return
	}

	page := c.Query("page")
	limit := c.Query("limit")
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

	users, err := h.userService.GetAllUsers(pageInt, limitInt, searchText)
	if err != nil {
		c.AbortWithError(http.StatusBadRequest, err)
		return
	}

	c.JSON(http.StatusOK, users)
}

func (h *UserHandler) AdminUpdateUser(c *gin.Context) {
	if role, exists := c.Get("role"); !exists || role != "Admin" {
		c.AbortWithStatusJSON(http.StatusForbidden, gin.H{"message": "You cannot change user"})
		return
	}

	idStr := c.Param("id")
	uID, err := strconv.Atoi(idStr)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Invalid user ID"})
		return
	}

	var req models.AdminUpdateUserRequest
	if err := c.BindJSON(&req); err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	if req.Password != "" {
		pwdHash, err := bcrypt.GenerateFromPassword([]byte(req.Password), bcrypt.DefaultCost)
		if err != nil {
			c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Failed to encrypt password"})
			return
		}
		req.Password = string(pwdHash)
	}

	err = h.userService.AdminUpdateUser(context.Background(), int32(uID), req)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Cannot update user"})
		return
	}
	c.JSON(http.StatusOK, gin.H{})
}

func (h *UserHandler) GetMe(c *gin.Context) {
	userID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "Unauthorized"})
		return
	}

	user, err := h.userService.GetUserById(c.Request.Context(), userID.(int32))
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Internal error"})
		return
	}
	if user == nil {
		c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "User not found"})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"id":      user.ID,
		"email":   user.Email,
		"name":    user.Name.String,
		"surname": user.Surname.String,
		"role":    user.Role,
	})
}

func (h *UserHandler) UpdateMe(c *gin.Context) {
	userID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "Unauthorized"})
		return
	}

	var req models.UpdateUserRequest
	if err := c.ShouldBindJSON(&req); err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	err := h.userService.UpdateUser(c.Request.Context(), userID.(int32), req)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Failed to update user"})
		return
	}

	c.JSON(http.StatusOK, gin.H{
		"message": "Profile updated successfully",
	})
}

func (h *UserHandler) GetProfile(c *gin.Context) {
	userID, exists := c.Get("id")
	if !exists {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "Unauthorized"})
		return
	}

	var profileDetails models.Profile

	detail := c.Query("detail")
	searchText := c.Query("search")
	switch detail {
	case "tournaments":
		tournaments, err := h.tournamentService.GetTournamentsProfile(userID.(int32), searchText)
		if err != nil {
			if err == errori.DBNotFound {
				break
			} else {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Failed to obtain tournaments"})
			}
			return
		}
		profileDetails.CreatedTournaments = tournaments
	case "teams":
		teams, err := h.teamService.GetTeamsProfile(userID.(int32), searchText)
		if err != nil {
			if err == errori.DBNotFound {
				break
			} else {
				c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Failed to obtain teams"})
			}
			return
		}

		for i := range teams {
			url, err := h.s3Service.GetPresignURL(fmt.Sprintf("team%d", teams[i].ID))
			if err == nil {
				teams[i].Image = url
			}
		}

		profileDetails.ManagedTeams = teams
	case "invites":
		invites, err := h.teamPlayerService.GetInvitations(userID.(int32))
		if err == nil {
			profileDetails.TeamInvites = invites
		}
	}

	c.JSON(http.StatusOK, profileDetails)
}

func (h *UserHandler) SearchUser(c *gin.Context) {
	searchText := c.Query("search")

	users, err := h.userService.SearchUserByEmail(context.Background(), searchText)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": "Failed to find users"})
		return
	}

	c.JSON(http.StatusOK, users)
}
