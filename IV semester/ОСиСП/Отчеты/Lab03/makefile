C = gcc

CFLAGS = -W -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -std=c11 -pedantic
.PHONY: clean

all: parent child

parent: parent.c makefile
	$(C) $(CFLAGS) parent.c -o parent

child: child.c makefile
	$(C) $(CFLAGS) child.c -o child

clean:
	rm parent child
