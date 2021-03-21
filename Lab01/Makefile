CC 		= gcc
CFLAGS 		= -Wall -pedantic -g
TARGETS		= bin/*

.PHONY : clear

all : es1.1 es1.2 es1.3
	@echo All Session 1 files compiled!

es1.1 : src/*
	$(CC) $(CFLAGS) src/strtoupper.c -o bin/strtoupper

es1.2 : src/*
	$(CC) $(CFLAGS) src/mystrcat.c -o bin/mystrcat

es1.3 : src/*
	$(CC) $(CFLAGS) src/cmdlineparsing.c -o bin/cmdlineparsing; \
	$(CC) $(CFLAGS) src/cmdlineparsing2.c -o bin/cmdlineparsing2; \
	$(CC) $(CFLAGS) src/cmdlineparsing3.c -o bin/cmdlineparsing3

clear : 
	-rm bin/*
