/**
 * IIS Project
 * @author Albert Tikaiev
 */
package middleware

import (
	"errors"
	"log"
	"net/http"

	apiErrors "backend/internal/errors"

	"github.com/gin-gonic/gin"
)

func ErrorHandler() gin.HandlerFunc {
	return func(c *gin.Context) {
		c.Next()

		if len(c.Errors) == 0 {
			return
		}

		err := c.Errors.Last().Err

		var apiErr apiErrors.APIError
		if errors.As(err, &apiErr) {
			c.JSON(apiErr.HTTPStatus, apiErr)
			return
		}

		// log real error for debugging
		log.Printf("Unhandled error: %+v", err)

		c.JSON(http.StatusInternalServerError, apiErrors.ErrInternal)
	}
}
