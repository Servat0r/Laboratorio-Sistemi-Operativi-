CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
SRC		= src
BIN		= bin
OBJS		= src/objs
LIB_DIRS	= src/libs
INCLUDE_DIRS	= src/includes
TARGETS		= $(BIN)/* $(LIB_DIRS)/* $(OBJS)/*
LIBRARY		= $(LIB_DIRS)/ -lutils

#Header files
headers := $(wildcard $(INCLUDE_DIRS)/*.h)
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDE_DIRS)/%.h=$(OBJS)/%.o) #This object is updated DYNAMICALLY !
#Executables that need library
exelibs := dummyshell
#All included files
includes := $(INCLUDE_DIRS:%=-I %)


.PHONY : clear
.SUFFIXES : .c .h .o

all : es6.1 es6.2 es6.3 es6.4 es6.5

debug :
	@echo $(headers)
	@echo $(objects)
	@echo $(exelibs)
	@echo $(includes)

#Generals

$(exelibs) : % : $(SRC)/%.c
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ -L $(LIBRARY)

% : $(SRC)/%.c
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ # -L $(LIBRARY)

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDE_DIRS)/%.h
	$(CC) $(includes) $(CFLAGS) -c $< -o $@

%.a : $(objects)
	ar rvs $(LIB_DIRS)/$@ $^
	
#Exercise 1

es6.1 : libutils.a dummyshell

#Exercise 2

es6.2 : sample_bg

#Exercise 3

es6.3 : zombieMaker

#Exercise 4

es6.4 : family

#Exercise 5

es6.5 : doFib

#Clearing

clear :
	-rm -f $(TARGETS)
