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
exelibs := es2


.PHONY: clear

.SUFFIXES: .c .h .o

all : es4.1 es4.2 es4.3

#Generals

$(exelibs) : % : $(SRC)/%.c
	$(CC) -I $(INCLUDES) $(CFLAGS) $< -o $(BIN)/$@ -L $(LIBRARY)

% : $(SRC)/%.c
	$(CC) $(CFLAGS) $< -o $(BIN)/$@

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDES)%.h
	$(CC) -I $(INCLUDES) $(CFLAGS) -c $< -o $@

%.a : $(objects)
	ar rvs $(LIB)/$@ $^

#Exercise 1

es4.1 : es1

es1 : $(SRC)/es1.c

#Exercise 2

es4.2 : es2

es2 : libutils.a $(SRC)/es2.c 

#Exercise 3

es4.3 : es3

es3 : $(SRC)/es3.c

#Clearing

clear : 
	-rm -f $(TARGETS)

