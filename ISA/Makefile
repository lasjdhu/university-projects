CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=gnu99
TARGET = dns

SRC = src/main.c src/helpers.c src/dns.c src/dns_utils.c
OBJ = $(SRC:.c=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c src/helpers.h src/dns.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
