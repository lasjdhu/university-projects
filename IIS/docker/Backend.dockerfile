# https://hub.docker.com/_/golang#how-to-use-this-image
FROM golang:1.25

WORKDIR /app

COPY backend/ /app

RUN go mod download

RUN go build -v -o /usr/local/bin/backend .

ENTRYPOINT ["backend"]
