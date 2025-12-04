/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package services

import (
	"backend/internal/errors"
	"backend/models"
	"context"
	"net/http"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgxpool"
	"golang.org/x/crypto/bcrypt"
)

type RegistrationService struct {
	db          *pgxpool.Pool
	userService *UserService
}

func NewRegistrationService(db *pgxpool.Pool, userService *UserService) *RegistrationService {
	return &RegistrationService{db, userService}
}

func (s *RegistrationService) RegisterUser(ctx context.Context, req models.RegisterUserRequest) (*models.RegisterUserResponse, error) {
	out := &models.RegisterUserResponse{}

	tx, err := s.db.BeginTx(ctx, pgx.TxOptions{})
	if err != nil {
		return nil, err
	}
	defer func() { _ = tx.Rollback(ctx) }()

	pwdHash, err := bcrypt.GenerateFromPassword([]byte(req.Password), bcrypt.DefaultCost)
	if err != nil {
		return nil, err
	}

	req.Password = string(pwdHash)
	response, err := s.userService.CreateUser(ctx, tx, req)
	if err != nil {
		return nil, &errors.APIError{
			Code:       "INTERNAL_ERROR",
			Message:    "Failed to create user",
			Err:        err,
			HTTPStatus: http.StatusBadRequest,
		}
	}

	if err := tx.Commit(ctx); err != nil {
		return nil, err
	}

	out.ID = response.ID
	out.Email = req.Email
	out.Name = req.Name
	out.Surname = req.Surname
	out.Role = response.Role

	return out, nil
}

func (s *RegistrationService) LoginUser(ctx context.Context, req models.LoginUserRequest) (*models.LoginUserResponse, error) {
	out := &models.LoginUserResponse{}

	response, err := s.userService.GetUserByEmail(ctx, req.Email)
	if err != nil {
		return nil, errors.DBNotFound
	}

	if err := bcrypt.CompareHashAndPassword([]byte(response.Password), []byte(req.Password)); err != nil {
		return nil, errors.InvalidPassword
	}

	out.ID = response.ID
	out.Email = response.Email
	out.Name = response.Name.String
	out.Surname = response.Surname.String
	out.Role = response.Role

	return out, nil
}

func (s *RegistrationService) ReloginUser(ctx context.Context, user_id int32) (*models.LoginUserResponse, error) {
	out := &models.LoginUserResponse{}

	response, err := s.userService.GetUserById(ctx, user_id)
	if err != nil {
		return nil, errors.DBNotFound
	}

	out.ID = response.ID
	out.Email = response.Email
	out.Name = response.Name.String
	out.Surname = response.Surname.String
	out.Role = response.Role

	return out, nil
}

func (s *RegistrationService) GetUserByID(ctx context.Context, user_id int32) (*models.LoginUserResponse, error) {
	out := &models.LoginUserResponse{}

	response, err := s.userService.GetUserById(ctx, user_id)
	if err != nil {
		return nil, errors.DBNotFound
	}

	out.ID = response.ID
	out.Email = response.Email
	out.Name = response.Name.String
	out.Surname = response.Surname.String
	out.Role = response.Role

	return out, nil
}
