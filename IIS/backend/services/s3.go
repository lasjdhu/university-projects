/**
 * IIS Project
 * @author Albert Tikaiev
 */
package services

import (
	"backend/internal/errors"
	"context"
	"io"
	"net/http"
	"os"

	"github.com/aws/aws-sdk-go-v2/aws"
	"github.com/aws/aws-sdk-go-v2/config"
	"github.com/aws/aws-sdk-go-v2/credentials"
	"github.com/aws/aws-sdk-go-v2/service/s3"
)

type S3Service struct {
	s3client    *s3.Client
	s3presigner *s3.PresignClient
}

func NewS3Service() *S3Service {
	cfg, err := config.LoadDefaultConfig(context.TODO(),
		config.WithRegion(os.Getenv("AWS_REGION")),
		config.WithCredentialsProvider(credentials.NewStaticCredentialsProvider(
			os.Getenv("AWS_ACCESS_KEY_ID"),
			os.Getenv("AWS_SECRET_ACCESS_KEY"),
			"",
		)),
	)

	if err != nil {
		return nil
	}

	client := s3.NewFromConfig(cfg)
	presigner := s3.NewPresignClient(client)

	return &S3Service{client, presigner}
}

func (s *S3Service) ImageExists(prefix string) (string, error) {
	resp, err := s.s3client.ListObjectsV2(context.Background(), &s3.ListObjectsV2Input{
		Bucket: aws.String(os.Getenv("AWS_BUCKET_NAME")),
		Prefix: aws.String("avatars/" + prefix),
	})

	if err != nil {
		return "", errors.Wrap(err, "Failed to list objects", http.StatusNotFound)
	}

	if len(resp.Contents) < 1 {
		return "", &errors.APIError{
			Code:       "NOT_FOUND",
			Message:    "Image not found",
			HTTPStatus: http.StatusNotFound,
		}
	}

	return *resp.Contents[0].Key, nil
}

func (s *S3Service) GetPresignURL(key string) (string, error) {
	image, err := s.ImageExists(key)

	if err != nil {
		return "", err
	}

	get, err := s.s3presigner.PresignGetObject(context.Background(), &s3.GetObjectInput{
		Bucket: aws.String(os.Getenv("AWS_BUCKET_NAME")),
		Key:    aws.String(image),
	})

	return get.URL, err
}

func (s *S3Service) PutObject(key, contentType string, contentLength int64, body io.Reader) error {
	_, err := s.s3client.PutObject(context.Background(), &s3.PutObjectInput{
		Bucket:        aws.String(os.Getenv("AWS_BUCKET_NAME")),
		Key:           aws.String(key),
		ContentType:   aws.String(contentType),
		ContentLength: aws.Int64(contentLength),
		Body:          body,
	})

	return err
}

func (s *S3Service) DeleteObject(key string) error {
	image, err := s.ImageExists(key)

	if err != nil {
		return &errors.APIError{
			Code:       "NOT_FOUND",
			Message:    "Failed to delete image",
			HTTPStatus: http.StatusNotFound,
		}
	}

	_, err = s.s3client.DeleteObject(context.Background(), &s3.DeleteObjectInput{
		Bucket: aws.String(os.Getenv("AWS_BUCKET_NAME")),
		Key:    aws.String(image),
	})

	if err != nil {
		return &errors.APIError{
			Code:       "NOT_FOUND",
			Message:    "Failed to delete image",
			HTTPStatus: http.StatusNotFound,
		}
	}

	return nil
}
