CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
TARGETS		= bin/*
INCLUDES	= ./includes/

.PHONY : clear

all : es5.1 es5.2 es5.3 es5.4

es5.1 : src/es1_sc.c src/es1_std.c
	$(CC) $(CFLAGS) $< -o bin/mycp_sc
	$(CC) $(CFLAGS) src/es1_std.c -o bin/mycp_std	

es5.2 : src/es2.c

printdir : src/printdir.c
	$(CC) $(CFLAGS) $< -o bin/printdir

es5.3 : src/myls.c
	$(CC) $(CFLAGS) $< -o bin/myls

es5.4 : src/es4.c src/printdir.c
	


clear :
	-rm -f $(TARGETS)

