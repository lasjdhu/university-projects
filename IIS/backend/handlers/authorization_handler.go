/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package handlers

import (
	errori "backend/internal/errors"
	"backend/internal/jwt"
	"backend/internal/validation"
	"backend/models"
	"backend/services"
	"errors"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
)

type AuthorizationHandler struct {
	registrationService *services.RegistrationService
}

func NewAuthorizationHandler(registrationService *services.RegistrationService) *AuthorizationHandler {
	return &AuthorizationHandler{
		registrationService: registrationService,
	}
}

func (h *AuthorizationHandler) Register(c *gin.Context) {
	createUserRequest := models.RegisterUserRequest{}
	if err := c.ShouldBindJSON(&createUserRequest); err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	registeredUser, err := h.registrationService.RegisterUser(c.Request.Context(), createUserRequest)
	if err != nil {
		if errors.Is(err, errori.InvalidLogin) {
			c.AbortWithStatusJSON(http.StatusBadRequest, gin.H{"message": err.Error()})
		} else {
			c.AbortWithStatusJSON(http.StatusInternalServerError, gin.H{"message": err.Error()})
		}
		return
	}

	c.JSON(http.StatusCreated, registeredUser)
}

func (h *AuthorizationHandler) Login(c *gin.Context) {
	loginUserRequest := models.LoginUserRequest{}
	if err := c.ShouldBindJSON(&loginUserRequest); err != nil {
		status, message := validation.BuildValidationErrorResponse(err)
		c.AbortWithStatusJSON(status, gin.H{"message": message})
		return
	}

	loginedUser, err := h.registrationService.LoginUser(c.Request.Context(), loginUserRequest)
	if err != nil {
		if errors.Is(err, errori.InvalidPassword) {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": err.Error()})
		}
		if errors.Is(err, errori.DBNotFound) {
			c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "Such user does not exist"})
		}
		return
	}

	access, refresh := jwt.GenerateAllTokens(loginedUser.ID, loginedUser.Role)

	// c.SetCookie("access", access, 15*60, "/", "", false, true)
	// c.SetCookie("refresh", refresh, 30*24*60*60, "/auth/refresh", "", false, true)
	http.SetCookie(c.Writer, &http.Cookie{
		Name:     "access",
		Value:    access,
		Path:     "/",
		MaxAge:   15 * 60,
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
	})

	http.SetCookie(c.Writer, &http.Cookie{
		Name:     "refresh",
		Value:    refresh,
		Path:     "/auth/refresh",
		MaxAge:   30 * 24 * 60 * 60,
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
	})

	c.JSON(http.StatusOK, loginedUser)
}

func (h *AuthorizationHandler) Refresh(c *gin.Context) {
	refresh, err := c.Cookie("refresh")
	if err != nil {
		c.AbortWithError(http.StatusUnauthorized, err)
		return
	}

	user_id, err := jwt.ValidateRefreshToken(refresh)
	if err != nil {
		c.AbortWithError(http.StatusUnauthorized, err)
		return
	}

	loginedUser, err := h.registrationService.ReloginUser(c.Request.Context(), user_id)
	if err != nil {
		c.AbortWithError(http.StatusUnauthorized, err)
		return
	}

	newAccess := jwt.GenerateAccessToken(loginedUser.ID, loginedUser.Role, time.Now().Add(time.Minute*15))
	// c.SetCookie("access", newAccess, 15*60, "/", "", false, true)
	http.SetCookie(c.Writer, &http.Cookie{
		Name:     "access",
		Value:    newAccess,
		Path:     "/",
		MaxAge:   15 * 60,
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
	})

	c.JSON(http.StatusOK, loginedUser)
}

func (h *AuthorizationHandler) Logout(c *gin.Context) {
	// c.SetCookie("access", "", -1, "/", "", false, true)
	// c.SetCookie("refresh", "", -1, "/auth/refresh", "", false, true)
	http.SetCookie(c.Writer, &http.Cookie{
		Name:     "access",
		Value:    "",
		Path:     "/",
		MaxAge:   -1,
		Expires:  time.Unix(0, 0),
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
	})

	http.SetCookie(c.Writer, &http.Cookie{
		Name:     "refresh",
		Value:    "",
		Path:     "/auth/refresh",
		MaxAge:   -1,
		Expires:  time.Unix(0, 0),
		HttpOnly: true,
		Secure:   true,
		SameSite: http.SameSiteNoneMode,
	})

	c.JSON(http.StatusOK, gin.H{"message": "Successfully logged out"})
}

func (h *AuthorizationHandler) GetMe(c *gin.Context) {
	access, err := c.Cookie("access")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "Access token missing"})
		return
	}

	user_id, _, err := jwt.ValidateAccessToken(access)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": err.Error()})
		return
	}

	user, err := h.registrationService.GetUserByID(c.Request.Context(), user_id)
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
		"name":    user.Name,
		"surname": user.Surname,
		"role":    user.Role,
	})
}
