CC		=  gcc
CFLAGS	        += -Wall -g -pedantic
SRC		= src
BIN		= bin
OBJS		= src/objs
LIB		= src/libs
INCLUDES	= src/includes/
TARGETS		= $(BIN)/* $(LIB)/* $(OBJS)/*
LIBRARY		= $(LIB)/ -ltokenizer

#Header files
headers := $(wildcard $(INCLUDES)/*.h)
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDES)/%.h=$(OBJS)/%.o) #This object is updated DYNAMICALLY !


.PHONY : clear
.SUFFIXES : .c .h .o

all : es3.1 es3.2 es3.3 es3.4 es3.5

#Generals

% : $(SRC)/%.c
	$(CC) -I $(INCLUDES) $(CFLAGS) $< -o $(BIN)/$@

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDES)%.h
	$(CC) -fPIC -I $(INCLUDES) $(CFLAGS) -c $< -o $@

%.a : $(objects)
	ar rvs $(LIB)/$@ $^

%.so : $(objects)
	$(CC) -shared -o $(LIB)/$@ $^
	
#Exercise 1

es3.1 : libtokenizer.a $(SRC)/tokenizer_main.c 
	$(CC) $(CFLAGS) -I $(INCLUDES) $(SRC)/tokenizer_main.c  -o $(BIN)/tokenizer_main_static -L $(LIBRARY)

#Exercise 2

es3.2 : libtokenizer.so $(SRC)/tokenizer_main.c 
	$(CC) $(CFLAGS) -I $(INCLUDES) $(SRC)/tokenizer_main.c  -o $(BIN)/tokenizer_main_shared -L $(LIBRARY)

#Exercise 3

es3.3 : es3

es3 : $(SRC)/es3.c

#Exercise 4

es3.4 : $(SRC)/es4.c
	$(CC) $(CFLAGS) $< -o $(BIN)/es4 -DINIT_VALUE=0

#Exerise 5

es3.5 : prog

prog : $(SRC)/prog.c

#Clearing

clear : 
	-rm -f $(TARGETS)

