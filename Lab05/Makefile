CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
TARGETS		= bin/* src/libs/* src/objs/*
INCLUDES	= -I ./includes/
LIBRARY		= libs/ -lutils

.PHONY : clear

all : es5.1 es5.2 es5.3 es5.4

es5.1 : src/es1_sc.c src/es1_std.c
	$(CC) $(CFLAGS) $< -o bin/mycp_sc
	$(CC) $(CFLAGS) src/es1_std.c -o bin/mycp_std	

es5.2 : src/myfind.c
	make lib5; \
	cd src; \
	$(CC) $(INCLUDES) $(CFLAGS) myfind.c -o ../bin/myfind -L $(LIBRARY) 

lib5 : src/queue.c src/myscandir.c
	cd src; \
	$(CC) $(INCLUDES) $(CFLAGS) -c queue.c -o objs/queue.o; \
	$(CC) $(INCLUDES) $(CFLAGS) -c myscandir.c -o objs/myscandir.o; \
	ar rvs libs/libutils.a objs/*.o 

printdir : src/printdir.c
	$(CC) $(CFLAGS) $< -o bin/printdir

es5.3 : src/myls.c
	$(CC) $(CFLAGS) $< -o bin/myls

es5.4 : src/lsdir.c
	make lib5; \
	cd src; \
	$(CC) $(INCLUDES) $(CFLAGS) lsdir.c -o ../bin/lsdir -L $(LIBRARY)
	
clear :
	-rm -f $(TARGETS)

