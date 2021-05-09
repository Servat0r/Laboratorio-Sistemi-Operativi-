CC 		= gcc
CFLAGS 		= -Wall -pedantic -g -std=c99
TARGETS		= bin/*

.PHONY : clear
.SUFFIXES : .c .h .o

all : es2.1 es2.2 es2.3 es2.4

#Generals
% : src/%.c
	$(CC) $(CFLAGS) $< -o bin/$@

#Exercise 1
es2.1 : es1

es1 : src/es1.c

#Exercise 2
es2.2 : es2

es2 : src/es2.c

#Exercise 3
es2.3 : es3

es3 : src/es3.c

#Exercise 4
es2.4 : es4 bes4

es4 : src/es4.c 

bes4 : src/bes4.c

#Clearing
clear :
	-rm bin/*
