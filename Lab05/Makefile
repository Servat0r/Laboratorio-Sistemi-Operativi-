CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
SRC		= src
BIN		= bin
OBJS		= src/objs
LIB		= src/libs
INCLUDES	= src/includes/
TARGETS		= $(BIN)/* $(LIB)/* $(OBJS)/*
LIBRARY		= $(LIB)/ -lutils

#Header files
headers := $(wildcard $(INCLUDES)/*.h)
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDES)/%.h=$(OBJS)/%.o) #This object is updated DYNAMICALLY !
#Executables that need library
exelibs := myfind lsdir


.PHONY : clear
.SUFFIXES : .c .h .o

all : es5.1 es5.2 es5.3 es5.4


#Generals

$(exelibs) : % : $(SRC)/%.c
	$(CC) -I $(INCLUDES) $(CFLAGS) $< -o $(BIN)/$@ -L $(LIBRARY)

% : $(SRC)/%.c
	$(CC) -I $(INCLUDES) $(CFLAGS) $< -o $(BIN)/$@ # -L $(LIBRARY)

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDES)%.h
	$(CC) -I $(INCLUDES) $(CFLAGS) -c $< -o $@

%.a : $(objects)
	ar rvs $(LIB)/$@ $^
	
#Exercise 1

es5.1 : es1_sc es1_std

#Exercise 2

es5.2 : libutils.a myfind

#Exercise 3

es5.3 : myls

#Exercise 4

es5.4 : libutils.a lsdir
	
#Clearing

clear :
	-rm -f $(TARGETS)

