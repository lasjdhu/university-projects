DROP TABLE IF EXISTS ParticipantStatistic CASCADE;
DROP TABLE IF EXISTS Match CASCADE;
DROP TABLE IF EXISTS Stage CASCADE;
DROP TABLE IF EXISTS TournamentParticipant CASCADE;
DROP TABLE IF EXISTS TeamPlayer CASCADE;
DROP TABLE IF EXISTS Tournament CASCADE;
DROP TABLE IF EXISTS Team CASCADE;
DROP TABLE IF EXISTS "User" CASCADE;

CREATE EXTENSION IF NOT EXISTS pg_trgm;

CREATE TABLE "User" (
    id SERIAL PRIMARY KEY,
    email VARCHAR NOT NULL UNIQUE,
    password VARCHAR NOT NULL,
    role VARCHAR CHECK ( role in ('Admin', 'Registered')) NOT NULL DEFAULT 'Registered',
    name VARCHAR,
    surname VARCHAR
);

CREATE TABLE Team(
    id SERIAL PRIMARY KEY,
    name VARCHAR NOT NULL ,
    description VARCHAR NOT NULL DEFAULT '',
    since DATE NOT NULL,
    manager_id INT REFERENCES "User"(id)
);

CREATE TABLE TeamPlayer(
    id SERIAL PRIMARY KEY,
    user_id INT REFERENCES "User"(id) NOT NULL,
    team_id INT REFERENCES Team(id) NOT NULL,
    since TIMESTAMP,
    until TIMESTAMP,
    state VARCHAR CHECK ( state in ('Invited', 'Active', 'Inactive')) NOT NULL DEFAULT 'Invited'
);

CREATE TABLE Tournament (
    id SERIAL PRIMARY KEY,
    name VARCHAR NOT NULL,
    discipline VARCHAR NOT NULL,
    expected_members INT NOT NULL,
    manager_id INT REFERENCES "User"(id),
    state VARCHAR CHECK ( state in ('Pending', 'Accepted', 'Rejected')) NOT NULL DEFAULT 'Pending',
    type VARCHAR CHECK ( type in ('Person', 'Team')) NOT NULL,
    prize INT,
    min_team_limit INT DEFAULT NULL,
    max_team_limit INT DEFAULT NULL
);

CREATE TABLE TournamentParticipant(
    id SERIAL PRIMARY KEY,
    state VARCHAR CHECK ( state in ('Pending', 'Accepted', 'Rejected')) NOT NULL DEFAULT 'Pending',
    team_id INT  REFERENCES Team(id),
    player_id INT REFERENCES "User"(id),
    tournament_id INT NOT NULL REFERENCES Tournament(id) ON DELETE CASCADE
);

CREATE TABLE Stage(
    id SERIAL PRIMARY KEY,
    tournament_id INT NOT NULL REFERENCES Tournament(id) ON DELETE CASCADE,
    level INT NOT NULL
);

CREATE TABLE Match(
    id SERIAL PRIMARY KEY,
    stage_id INT NOT NULL REFERENCES Stage(id) ON DELETE CASCADE,
    next_match_id INT REFERENCES Match(id),
    name VARCHAR NOT NULL,
    first_participant_id INT REFERENCES TournamentParticipant(id),
    first_participant_result_text VARCHAR,
    first_participant_is_winner BOOLEAN DEFAULT FALSE,
    second_participant_id INT REFERENCES TournamentParticipant(id),
    second_participant_result_text VARCHAR,
    second_participant_is_winner BOOLEAN DEFAULT FALSE,
    "date" TIMESTAMP
);
