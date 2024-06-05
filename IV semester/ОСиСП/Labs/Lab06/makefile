C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

all: sort_index generate_index

sort_index: sort_index.c makefile
	$(C) $(CFLAGS) sort_index.c -o sort_index

generate_index: generate_index.c makefile
	$(C) $(CFLAGS) generate_index.c -o generate_index

clean:
	rm -f sort_index generate_index
