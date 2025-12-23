/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev, Dmitrii Ivanushkin
 */
package services

import (
	"backend/models"
	"context"

	"github.com/jackc/pgx/v5"
	"github.com/jackc/pgx/v5/pgxpool"
)

type UserService struct {
	db *pgxpool.Pool
}

func NewUserService(db *pgxpool.Pool) *UserService {
	return &UserService{db}
}

func (s *UserService) CountUsers(searchText string) (int, error) {
	var count int
	err := s.db.QueryRow(context.Background(), `SELECT COUNT(*) FROM "User"
	WHERE role = 'Registered' AND ($1 = '' OR similarity(name || ' ' || surname, $1) > 0.05)`, searchText).Scan(&count)
	return count, err
}

func (s *UserService) GetAllUsers(page, limit int, searchText string) (models.PaginationAnswer[models.User], error) {
	ctx := context.Background()
	offset := (page - 1) * limit
	var ans models.PaginationAnswer[models.User]

	total, err := s.CountUsers(searchText)
	if err != nil {
		return ans, err
	}

	rows, err := s.db.Query(ctx, `
	SELECT id, email, role, name, surname FROM "User"
	WHERE role = 'Registered' AND ($1 = '' OR similarity(name || ' ' || surname, $1) > 0.05)
	ORDER BY CASE WHEN $1 = '' THEN id ELSE similarity(name, $1) END DESC
	LIMIT $2 OFFSET $3
	`, searchText, limit, offset)
	if err != nil {
		return ans, err
	}
	defer rows.Close()

	var users []models.User
	for rows.Next() {
		var user models.User
		if err := rows.Scan(
			&user.ID,
			&user.Email,
			&user.Role,
			&user.Name,
			&user.Surname); err != nil {
			return ans, err
		}
		users = append(users, user)
	}

	ans = models.PaginationAnswer[models.User]{
		Data:         users,
		TotalRecords: total,
		TotalPages:   (total + limit - 1) / limit,
		CurrentPage:  page,
		Limit:        limit,
	}

	return ans, nil
}

func (s *UserService) GetUserByEmail(ctx context.Context, email string) (*models.User, error) {
	var user models.User

	err := s.db.QueryRow(ctx,
		`SELECT id, email, password, role, name, surname FROM "User" WHERE email=$1`, email,
	).Scan(&user.ID, &user.Email, &user.Password, &user.Role, &user.Name, &user.Surname)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, nil
		}
		return nil, err
	}

	return &user, nil
}

func (s *UserService) GetUserById(ctx context.Context, id int32) (*models.User, error) {
	var user models.User

	err := s.db.QueryRow(ctx,
		`SELECT id, email, password, role, name, surname FROM "User" WHERE id=$1`, id,
	).Scan(&user.ID, &user.Email, &user.Password, &user.Role, &user.Name, &user.Surname)

	if err != nil {
		if err == pgx.ErrNoRows {
			return nil, nil
		}
		return nil, err
	}

	return &user, nil
}

func (s *UserService) CreateUser(ctx context.Context, tx pgx.Tx, req models.RegisterUserRequest) (*models.RegisterUserResponse, error) {
	var out models.RegisterUserResponse

	err := tx.QueryRow(ctx,
		`INSERT INTO "User"(email, password, name, surname) VALUES($1,$2,$3,$4) RETURNING id, role`,
		req.Email, req.Password, req.Name, req.Surname,
	).Scan(&out.ID, &out.Role)

	if err != nil {
		return nil, err
	}

	out.Email = req.Email
	out.Name = req.Name
	out.Surname = req.Surname

	return &out, nil
}

func (s *UserService) AdminUpdateUser(ctx context.Context, id int32, req models.AdminUpdateUserRequest) error {
	var err error
	if req.Password == "" {
		_, err = s.db.Exec(ctx, `
		UPDATE "User"
		SET email = $1
		WHERE id = $2
	`, req.Email, id)
	} else {
		_, err = s.db.Exec(ctx, `
		UPDATE "User"
		SET email = $1, password = $2
		WHERE id = $3
	`, req.Email, req.Password, id)
	}

	return err
}

func (s *UserService) UpdateUser(ctx context.Context, id int32, req models.UpdateUserRequest) error {
	_, err := s.db.Exec(ctx, `
		UPDATE "User"
		SET name = $1, surname = $2, email = $3
		WHERE id = $4
	`, req.Name, req.Surname, req.Email, id)

	return err
}

func (s *UserService) SearchUserByEmail(ctx context.Context, text string) ([]models.BaseUser, error) {
	var users []models.BaseUser

	rows, err := s.db.Query(ctx, `
		SELECT id, email, name, surname FROM "User"
    WHERE Role = 'Registered' AND similarity(email, $1) > 0.05
    ORDER BY similarity(email, $1) DESC
    LIMIT 10
	`, text)
	if err != nil {
		return users, err
	}
	defer rows.Close()

	for rows.Next() {
		var user models.BaseUser
		err = rows.Scan(
			&user.ID,
			&user.Email,
			&user.Name,
			&user.Surname,
		)
		if err != nil {
			return users, err
		}
		users = append(users, user)
	}

	return users, nil
}
