CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm
FILES=hashtable.c test.c test_util.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

run: test
	@./test > current-test.output
	@echo "\nTest output differences:"
	@diff -su ht.out current-test.output
	@rm current-test.output

clean:
	rm -f test
