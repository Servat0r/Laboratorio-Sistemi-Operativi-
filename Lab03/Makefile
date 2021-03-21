CC 		= gcc
CFLAGS 		= -Wall -pedantic -g
TARGETS		= bin/* bin/es1-2/* src/es1-2/mylibs/* src/es1-2/objs/*
INCLUDES	= -I ./includes/
LIBRARY		= -L mylibs/ -ltokenizer

.PHONY : clear

all : es3.1 es3.2 es3.3 es3.4
	@echo All Session 3 files compiled!

es3.1 : src/es1-2/*
	cd src/es1-2 ; \
	$(CC) $(CFLAGS) $(INCLUDES) -c src/tokenizer_lib.c -o objs/tokenizer_lib.o; \
	ar rvs mylibs/libtokenizer.a objs/tokenizer_lib.o; \
	$(CC) $(CFLAGS) $(INCLUDES) src/tokenizer_main.c -o ../../bin/es1-2/tokenizer_main_static $(LIBRARY)
	@echo Creato eseguibile con libreria statica	

es3.2 : src/es1-2/*
	cd src/es1-2; \
	$(CC) $(CFLAGS) $(INCLUDES) -fPIC -c src/tokenizer_lib.c -o objs/tokenizer_shared_lib.o; \
	$(CC) -shared -o mylibs/libtokenizer.so objs/tokenizer_shared_lib.o; \
	$(CC) $(CFLAGS) $(INCLUDES) src/tokenizer_main.c -o ../../bin/es1-2/tokenizer_main_shared $(LIBRARY)
	@echo Creato eseguibile con libreria condivisa

es3.3 : src/es3.c
	$(CC) $(CFLAGS) $^ -o bin/es3 

es3.4 : src/es4.c
	$(CC) $(CFLAGS) $^ -o bin/es4 -DINIT_VALUE=0

es3.5 : src/prog.c
	$(CC) $(CFLAGS) $^ -o bin/prog

clear : 
	-rm -f $(TARGETS)

