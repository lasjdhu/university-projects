/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package errors

import (
	"errors"
	"net/http"
)

var InvalidPassword = errors.New("Invalid password")
var InvalidLogin = errors.New("User with this email already exists")
var InternalError = errors.New("Internal error")
var DBNotFound = errors.New("No rows")
var NotAcceptable = errors.New("Not acceptable")

type APIError struct {
	Code       string `json:"code"`
	Message    string `json:"message"`
	HTTPStatus int    `json:"-"`
	Err        error  `json:"-"`
}

func (e APIError) Error() string {
	return e.Message
}

func (e APIError) Unwrap() error {
	return e.Err
}

func Wrap(err error, msg string, status int) APIError {
	return APIError{
		Code:       "INTERNAL_ERROR",
		Message:    msg,
		HTTPStatus: status,
		Err:        err,
	}
}

var (
	ErrNotFound = APIError{
		Code:       "NOT_FOUND",
		Message:    "Resourse not found",
		HTTPStatus: http.StatusNotFound,
	}
	ErrInternal = APIError{
		Code:       "INTERNAL_ERROR",
		Message:    "Internal server error",
		HTTPStatus: http.StatusInternalServerError,
	}
)
