CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
SRC		= src
BIN		= bin
OBJS		= src/objs
LIB_DIRS	= src/libs
INCLUDE_DIRS	= src/includes
TARGETS		= $(BIN)/* $(LIB_DIRS)/* $(OBJS)/*
LIBRARY		= $(LIB_DIRS)/ -lutils
LTHREAD		= -lpthread

#Header files
headers := $(wildcard $(INCLUDE_DIRS)/*.h)
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDE_DIRS)/%.h=$(OBJS)/%.o) #This object is updated DYNAMICALLY !
#Executables that need library
exelibs := test_tsqueue multiple_pc
#All included files
includes := $(INCLUDE_DIRS:%=-I %)

.PHONY : clear
.SUFFIXES : .c .h .o

all : es8.1 es8.2 es8.3

#Generals

$(exelibs) : % : $(SRC)/%.c
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ $(LTHREAD) -L $(LIBRARY)

% : $(SRC)/%.c
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ $(LTHREAD) # -L $(LIBRARY)

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDE_DIRS)/%.h
	$(CC) $(includes) $(CFLAGS) -c $< -o $@ $(LTHREAD)

%.a : $(objects)
	ar rvs $(LIB_DIRS)/$@ $^

#Ex1
es8.1 : libutils.a test_tsqueue multiple_pc

#Ex2
es8.2 : mybc

#Ex3
es8.3 : dec pipedec

clear :
	-rm $(TARGETS)

