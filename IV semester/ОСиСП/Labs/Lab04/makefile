C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic

.PHONY: clean

all: main producer consumer

main: main.c makefile
	$(C) $(CFLAGS) main.c -o main

producer: producer.c makefile
	$(C) $(CFLAGS) producer.c -o producer

consumer: consumer.c makefile
	$(C) $(CFLAGS) consumer.c -o consumer

clean:
	rm -f main producer consumer
