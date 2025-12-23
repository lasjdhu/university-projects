/**
 * IIS Project
 * @author Albert Tikaiev
 */
package middleware

import (
	"backend/internal/jwt"
	"net/http"

	"github.com/gin-gonic/gin"
)

func JWTAuthMiddleware(c *gin.Context) {
	access, err := c.Cookie("access")
	if err != nil {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": "You must be authorized"})
		return
	}

	id, role, err := jwt.ValidateAccessToken(access)
	if err != nil {
		c.AbortWithStatusJSON(http.StatusUnauthorized, gin.H{"message": err.Error()})
		return
	}

	c.Set("id", id)
	c.Set("role", role)

	c.Next()
}
