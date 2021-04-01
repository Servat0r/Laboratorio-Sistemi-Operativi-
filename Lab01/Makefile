CC 		= gcc
CFLAGS 		= -Wall -pedantic -g
TARGETS		= bin/*

.PHONY : clear

all : es1.1 es1.2 es1.3
	@echo All Session 1 files compiled!

#Generals
% : src/%.c
	$(CC) $(CFLAGS) $^ -o bin/$@

#Exercise 1
es1.1 : strtoupper

strtoupper : src/strtoupper.c

#Exercise 2
es1.2 : mystrcat

mystrcat : src/mystrcat.c

#Exercise 3
es1.3 : cmdlineparsing cmdlineparsing2 cmdlineparsing3

cmdlineparsing : src/cmdlineparsing.c

cmdlineparsing2 : src/cmdlineparsing2.c

cmdlineparsing3 : src/cmdlineparsing3.c

#Clearing
clear : 
	-rm bin/*
