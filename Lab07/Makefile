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
exelibs := pc_arraystack pc_arraystack_for pc_boundedqueue philosophers pipeline3
#All included files
includes := $(INCLUDE_DIRS:%=-I %)

.PHONY : clear
.SUFFIXES : .c .h .o

all : es7.1 es7.2 es7.3 es7.4

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
es7.1 : libutils.a pc_arraystack pc_arraystack_for pc_boundedqueue

#Ex2
es7.2 : libutils.a philosophers

#Ex3
es7.3 : libutils.a pipeline3

#Ex4
es7.4 : libutils.a pipeline4


clear :
	-rm $(TARGETS)

