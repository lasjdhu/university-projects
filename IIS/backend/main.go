/**
 * IIS Project
 * @author Albert Tikaiev, Dias Tursynbayev
 */
package main

import (
	"backend/handlers"
	"backend/internal/middleware"
	"backend/services"
	"context"
	"os"
	"strings"
	"time"

	"github.com/gin-contrib/cors"
	"github.com/gin-gonic/gin"
	"github.com/jackc/pgx/v5/pgxpool"
	"github.com/joho/godotenv"
)

func main() {
	godotenv.Load()
	ctx := context.Background()

	dbURL := os.Getenv("DATABASE_URL")
	config, err := pgxpool.ParseConfig(dbURL)
	if err != nil {
		panic(err)
	}

	config.MaxConns = 20
	config.MinConns = 2
	config.MaxConnIdleTime = 5 * time.Minute

	dbPool, err := pgxpool.NewWithConfig(ctx, config)
	if err != nil {
		panic(err)
	}
	defer dbPool.Close()

	splitted_origins := []string{}
	if origins_env := os.Getenv("CORS_ORIGIN"); origins_env != "" {
		splitted_origins = strings.Split(origins_env, ",")
	}

	router := gin.Default()
	router.Use(cors.New(cors.Config{
		AllowOrigins:     splitted_origins,
		AllowMethods:     []string{"GET", "POST", "PUT", "DELETE"},
		AllowHeaders:     []string{"Origin", "Content-Type", "Authorization"},
		ExposeHeaders:    []string{"Content-Length"},
		AllowCredentials: true,
	}))
	router.Use(middleware.ErrorHandler())

	s3Service := services.NewS3Service()
	userService := services.NewUserService(dbPool)
	teamService := services.NewTeamService(dbPool)
	tournamentService := services.NewTournamentService(dbPool)
	tournamentParticipantService := services.NewTournamentParticipantService(dbPool)
	registrationService := services.NewRegistrationService(dbPool, userService)
	matchService := services.NewMatchService(dbPool)
	teamPlayerService := services.NewTeamPlayerService(dbPool)

	userHandler := handlers.NewUserHandler(userService, matchService, teamService, tournamentService, teamPlayerService, s3Service)
	authHandler := handlers.NewAuthorizationHandler(registrationService)
	overviewHandler := handlers.NewOverviewHandler(tournamentParticipantService, tournamentService, teamService, s3Service)
	teamHandler := handlers.NewTeamHandler(teamService, s3Service, teamPlayerService, userService)
	tournamentHandler := handlers.NewTournamentHandler(tournamentService)
	tournamentParticipantHandler := handlers.NewTournamentParticipantHandler(tournamentParticipantService, tournamentService, teamService)
	matchHandler := handlers.NewMatchHandler(matchService)

	// Team endpoints
	router.GET("/teams", teamHandler.GetTeams)
	router.POST("/teams", middleware.JWTAuthMiddleware, teamHandler.CreateTeam)
	router.GET("/teams/:id", teamHandler.GetTeamById)
	router.PUT("/teams/:id", middleware.JWTAuthMiddleware, teamHandler.UpdateTeam)
	router.POST("/teams/:id/invite", middleware.JWTAuthMiddleware, teamHandler.InvitePlayer)
	router.PUT("/teams/:id/invite", middleware.JWTAuthMiddleware, teamHandler.ResolveInvite)
	router.PUT("/teams/:id/avatar", middleware.JWTAuthMiddleware, teamHandler.UpdateTeamAvatar)
	router.PUT("/teams/:id/players/:pid/state", middleware.JWTAuthMiddleware, teamHandler.ChangePlayerState)

	// Tournament endpoints
	router.GET("/tournaments", tournamentHandler.GetTournaments)
	router.GET("/tournaments/:id", tournamentHandler.GetTournamentById)
	router.GET("/tournaments/:id/bracket", tournamentHandler.GetTournamentBracket)
	router.PUT("/tournaments/:id/bracket", middleware.JWTAuthMiddleware, tournamentHandler.UpdateTournamentBracket)
	router.POST("/tournaments/:id/participants", middleware.JWTAuthMiddleware, tournamentParticipantHandler.CreateParticipant)
	router.PUT("/tournaments/:id/participants", middleware.JWTAuthMiddleware, tournamentParticipantHandler.ResolveParticipant)
	router.POST("/tournaments", middleware.JWTAuthMiddleware, tournamentHandler.CreateTournament)
	router.PUT("/tournaments/:id", middleware.JWTAuthMiddleware, tournamentHandler.UpdateTournament)
	router.DELETE("/tournaments/:id", middleware.JWTAuthMiddleware, tournamentHandler.DeleteTournament)
	router.POST("/tournaments/:id/start", middleware.JWTAuthMiddleware, tournamentHandler.StartTournament)

	// Misc
	router.GET("/players", tournamentParticipantHandler.GetPlayers)
	router.GET("/players/:id", tournamentParticipantHandler.GetPlayerById)
	router.GET("/user", userHandler.SearchUser)
	router.GET("/matches", matchHandler.GetMatches)
	router.GET("/overview", overviewHandler.GetOverview)

	userGroup := router.Group("/user")
	userGroup.Use(middleware.JWTAuthMiddleware)
	userGroup.GET("/profile/me", userHandler.GetMe)
	userGroup.GET("/profile/details", userHandler.GetProfile)
	userGroup.PUT("/profile/me", userHandler.UpdateMe)

	adminGroup := router.Group("/admin")
	adminGroup.Use(middleware.JWTAuthMiddleware)
	adminGroup.GET("/users", userHandler.GetAllUsers)
	adminGroup.PUT("/users/:id", userHandler.AdminUpdateUser)
	adminGroup.GET("/tournaments", tournamentHandler.GetAdminTournaments)
	adminGroup.PUT("/tournaments/state", tournamentHandler.UpdateTournamentState)

	authUser := router.Group("/auth")
	authUser.POST("/register", authHandler.Register)
	authUser.POST("/login", authHandler.Login)
	authUser.POST("/refresh", authHandler.Refresh)
	authUser.POST("/logout", authHandler.Logout)
	authUser.GET("/user/me", middleware.JWTAuthMiddleware, authHandler.GetMe)

	if err := router.Run(":8080"); err != nil {
		panic(err)
	}
}
