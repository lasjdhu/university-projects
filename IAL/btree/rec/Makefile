CC=gcc
CFLAGS=-Wall -std=c11 -pedantic -lm
FILES=btree.c ../btree.c ../test_util.c ../test.c

.PHONY: test clean

test: $(FILES)
	$(CC) $(CFLAGS) -o $@ $(FILES)

run: test
	@./test > current-test.output
	@echo "\nTest output differences:"
	@diff -su ../btree.out current-test.output
	@rm current-test.output

clean:
	rm -f test
