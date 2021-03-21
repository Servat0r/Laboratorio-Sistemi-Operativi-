CC 		= gcc
CFLAGS 		= -Wall -pedantic -g
TARGETS		= bin/*

.PHONY : clear

all : es2.1 es2.2 es2.3 es2.4

es2.1 : src/es1.c
	$(CC) $(CFLAGS) $^ -o bin/es1

es2.2 : src/es2.c
	$(CC) $(CFLAGS) $^ -o bin/es2

es2.3 : src/es3.c
	$(CC) $(CFLAGS) $^ -o bin/es3

es2.4 : src/es4.c src/bes4.c
	$(CC) $(CFLAGS) src/es4.c -o bin/es2; \
	$(CC) $(CFLAGS) src/bes4.c -o bin/bes2

clear :
	-rm bin/*
