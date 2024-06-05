C = gcc

CFLAGS = -c -W -Wall -std=c11

dirwalk: oslw1.o
	$(C) oslw1.o -o dirwalk

oslw1.o: oslw1.c
	$(C) $(CFLAGS) -c oslw1.c

clean:
	rm *.o dirwalk
