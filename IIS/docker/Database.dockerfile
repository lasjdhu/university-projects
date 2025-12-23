FROM postgres:latest

# https://docs.docker.com/guides/pre-seeding/#pre-seed-the-database-by-bind-mounting-a-sql-script
COPY database/init.sql /docker-entrypoint-initdb.d/1.sql
COPY database/sample.sql /docker-entrypoint-initdb.d/2.sql
