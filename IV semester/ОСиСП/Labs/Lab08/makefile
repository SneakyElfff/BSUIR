C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

all: server client

server: server.c makefile
	$(C) $(CFLAGS) server.c -o server

client: client.c makefile
	$(C) $(CFLAGS) client.c -o client

clean:
	rm -f server client
