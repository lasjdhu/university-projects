/**
 * IIS Project
 * @author Albert Tikaiev
 */
package validation

import (
	"net/http"
	"strings"

	"github.com/go-playground/validator/v10"
)

func BuildValidationErrorResponse(err error) (int, string) {
	if verrs, ok := err.(validator.ValidationErrors); ok {
		error_fieds := []string{}
		for _, verr := range verrs {
			error_fieds = append(error_fieds, verr.StructField())
		}
		return http.StatusBadRequest, "Bad fields were provided:" + strings.Join(error_fieds, ", ")
	}

	return http.StatusInternalServerError, "Internal server error"
}
