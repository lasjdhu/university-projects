CC=gcc
CFLAGS=--std=c99 -Wall

TARGET=main

HEADERS := cnf.h
OBJECTS := main.o add_conditions.o


default: $(TARGET)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -Wall -o $@

clean:
	-rm -f $(OBJECTS)
	-rm -f $(TARGET)

test: default
	@python3 ../tests/run_tests.py
