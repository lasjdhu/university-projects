CC=gcc
CFLAGS=-std=gnu99 -Wall -Wextra -Werror -pedantic

all: proj2

proj2: proj2.o
	$(CC) $(CFLAGS) -o $@ $^

proj2.o: proj2.c proj2.h
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f proj2 proj2.o
