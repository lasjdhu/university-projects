/**
 * IIS Project
 * @author Albert Tikaiev
 */
package jwt

import (
	"errors"
	"os"
	"strconv"
	"time"

	"github.com/golang-jwt/jwt/v5"
)

func GenerateAllTokens(id int32, role string) (string, string) {
	now := time.Now()

	return GenerateAccessToken(id, role, now.Add(time.Minute*15)), generateRefreshToken(id, now.Add(time.Hour*24*30))
}

func GenerateAccessToken(id int32, role string, exp time.Time) string {
	access_token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.MapClaims{
		"sub":  strconv.Itoa(int(id)),
		"exp":  &jwt.NumericDate{exp},
		"role": role,
	})

	access_token_string, _ := access_token.SignedString([]byte(os.Getenv("JWT_SECRET")))

	return access_token_string
}

func generateRefreshToken(id int32, exp time.Time) string {
	refresh_token := jwt.NewWithClaims(jwt.SigningMethodHS256, jwt.RegisteredClaims{
		Subject:   strconv.Itoa(int(id)),
		ExpiresAt: &jwt.NumericDate{exp},
	})

	retfresh_token_string, _ := refresh_token.SignedString([]byte(os.Getenv("JWT_SECRET")))
	return retfresh_token_string
}

func ValidateRefreshToken(refresh_string string) (int32, error) {
	token, err := jwt.Parse(refresh_string, func(t *jwt.Token) (any, error) {
		return []byte(os.Getenv("JWT_SECRET")), nil
	})
	if err != nil {
		return -1, err
	}

	exp, err := token.Claims.GetExpirationTime()
	if err != nil {
		return -1, errors.New("Invalid token")
	}

	if time.Now().After(exp.Time) {
		return -1, errors.New("Refresh token expired")
	}

	id, err := subjectAsInt32(token.Claims)

	return id, err
}

func ValidateAccessToken(access_string string) (int32, string, error) {
	token, err := jwt.Parse(access_string, func(t *jwt.Token) (any, error) {
		return []byte(os.Getenv("JWT_SECRET")), nil
	})
	if err != nil {
		return -1, "", err
	}

	claims, ok := token.Claims.(jwt.MapClaims)
	if !ok {
		return -1, "", errors.New("Invalid token")
	}

	exp, err := claims.GetExpirationTime()
	if err != nil {
		return -1, "", errors.New("Invalid token")
	}
	if time.Now().After(exp.Time) {
		return -1, "", errors.New("Access token expired")
	}

	id, err := subjectAsInt32(token.Claims)

	return id, claims["role"].(string), err
}

func subjectAsInt32(claims jwt.Claims) (int32, error) {
	sub, err := claims.GetSubject()
	if err != nil {
		return -1, errors.New("Invalid token")
	}

	id, err := strconv.ParseInt(sub, 10, 32)
	if err != nil {
		return -1, errors.New("Invalid token")
	}
	return int32(id), nil
}
