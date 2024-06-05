C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

all: main

main: main.c makefile
	$(C) $(CFLAGS) main.c -o main

clean:
	rm -f main
