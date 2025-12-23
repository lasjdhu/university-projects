# Student tournaments IS

## November 2025

## Authors

- ![@prosperritty](https://github.com/prosperritty)
- ![@lasjdhu](https://github.com/lasjdhu)
- ![@m3m01r](https://github.com/m3m01r)

### Description

This project is a comprehensive full-stack web application designed to
facilitate the management of sports or e-sports tournaments. It functions as a
central hub for administrators, team managers, and players to organize events,
track match results, and manage team compositions.

### Implementation

Architecture & Backend The backend is built as a RESTful API using Go (Golang)
and the Gin framework. It is structured into modular packages (handlers,
services, models) to separate business logic from HTTP routing. Data persistence
is handled by a PostgreSQL database using the pgx driver for high-performance
connection pooling.

- Authentication: Security is implemented via JWT (JSON Web Tokens) using a
  dual-token strategy (short-lived Access tokens and long-lived Refresh tokens
  stored in HttpOnly cookies) and bcrypt for password hashing.
- Storage: The system integrates with Amazon S3 for handling file uploads.

Frontend The user interface is a responsive Single Page Application (SPA)
developed with React. It utilizes TailwindCSS for utility-first styling and
TanStack Query for efficient asynchronous state management and data caching.

- Key Libraries: Communication with the API is managed by Axios (with
  interceptors for auto-token refreshing). Form validation relies on Zod and
  React Hook Form.
- Visualization: Tournament progression is visualized using the Bracketry
  library, allowing for dynamic display and modification of tournament trees.

Key Features

- Role-Based Access: Distinction between Administrators (system-wide control)
  and Registered Users (Team/Tournament managers).
- Tournament Management: Creation of tournaments, configuring parameters, and
  managing match outcomes via an interactive bracket.
- Team & User Management: Registration, team creation, player assignment, and
  profile management.
- Deployment: The application supports containerized deployment using Docker and
  Docker Compose.
