/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package handlers

import (
	"backend/models"
	"backend/services"
	"log"
	"math"
	"net/http"
	"strconv"

	"github.com/gin-gonic/gin"
)

type MatchHandler struct {
	matchService *services.MatchService
}

func NewMatchHandler(matchService *services.MatchService) *MatchHandler {
	return &MatchHandler{matchService}
}

func (h *MatchHandler) GetMatches(c *gin.Context) {
	page := c.Query("page")
	limit := c.Query("limit")
	team := c.Query("team")
	user := c.Query("user")
	search := c.Query("search")

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
	offset := (pageInt - 1) * limitInt

	if team != "" && user != "" {
		c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "URL parameter cannot specify both team and player"})
		return
	}

	var paginated models.PaginationAnswer[models.MatchDetailed]
	var matches []models.MatchDetailed
	var count int

	if team != "" {
		teamID, err := strconv.Atoi(team)
		if err != nil || teamID < 1 {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Wrong team ID"})
			return
		}
		count, err = h.matchService.CountTeamMatches(int32(teamID))
		if err != nil {
			c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches for this team"})
			return
		}
		if count > 0 {
			matches, err = h.matchService.GetMatchesProfileTeam(int32(teamID), int32(limitInt), int32(offset))
			if err != nil {
				c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches for this team"})
				return
			}
		}
	} else if user != "" {
		userID, err := strconv.Atoi(user)
		if err != nil || userID < 1 {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": "Wrong team ID"})
			return
		}

		count, err = h.matchService.CountPlayerMatches(int32(userID), search)
		if err != nil {
			c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches for this player"})
			return
		}
		if count > 0 {
			matches, err = h.matchService.GetMatchesProfilePlayer(int32(userID), int32(limitInt), int32(offset), search)
			if err != nil {
				c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches for this player"})
				return
			}
		}
	} else {
		var err error
		count, err = h.matchService.CountMatches()
		if err != nil {
			c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches"})
			return
		}
		if count > 0 {
			matches, err = h.matchService.GetMatches(int32(limitInt), int32(offset))
			log.Println(err)
			if err != nil {
				c.AbortWithStatusJSON(http.StatusNotFound, gin.H{"message": "Cannot found matches"})
				return
			}
		}
	}

	if count > 0 {
		paginated.Data = matches
	} else {
		paginated.Data = make([]models.MatchDetailed, 0)
	}
	paginated.TotalRecords = count
	paginated.CurrentPage = pageInt
	paginated.Limit = limitInt
	paginated.TotalPages = int(math.Ceil(float64(count) / float64(limitInt)))

	c.JSON(http.StatusOK, paginated)
}
