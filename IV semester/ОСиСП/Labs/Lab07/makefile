C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

all: main_thread concurrency

main_thread: main_thread.c makefile
	$(C) $(CFLAGS) main_thread.c -o main_thread

concurrency: concurrency.c makefile
	$(C) $(CFLAGS) concurrency.c -o concurrency

clean:
	rm -f main_thread concurrency
