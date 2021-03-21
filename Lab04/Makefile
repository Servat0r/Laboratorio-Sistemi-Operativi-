CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
TARGETS		= bin/* bin/es2/* src/es2/*.o src/es2/lib/*
INCLUDES	= -I src/es2/
LIBRARY		= -L src/es2/lib/ -lutils

.PHONY: clear

.SUFFIXES: .c .h

all : es4.1 es4.2 es4.3

es4.1 : src/es1.c
	-$(CC) $(CFLAGS) $^ -o bin/es1

es4.2 : es4.2.library
	-$(CC) $(INCLUDES) $(CFLAGS) src/es2/es2.c -o bin/es2/es2 $(LIBRARY)

es4.2.library : es4.2.objects src/es2/numfuncs.o
	-ar rvs src/es2/lib/libutils.a src/es2/numfuncs.o

es4.2.objects : src/es2/numfuncs.c src/es2/numfuncs.h
	-$(CC) $(INCLUDES) $(CFLAGS) -c src/es2/numfuncs.c -o src/es2/numfuncs.o

es4.3 : src/es3.c
	-$(CC) $(CFLAGS) $^ -o bin/mywc

clear : 
	-rm -f $(TARGETS)
