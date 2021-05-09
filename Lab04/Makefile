CC				=  gcc
CFLAGS			+= -Wall -g -pedantic  -std=c99
#Exercise-only dirs
SRC				= src
BIN				= bin
OBJS			= src/objs
LIB_DIRS		= src/libs
INCLUDE_DIRS	= src/includes
LIBRARY			= $(LIB_DIRS)/ -lutils
#Shared dirs and objects between ALL exercises
SHAREDIR		= ../shared
SHARESRC		= $(SHAREDIR)/src
SHARELIB		= $(SHAREDIR)/src/lib
SHAREBIN		= $(SHAREDIR)/bin
SHAREOBJS		= $(SHAREDIR)/src/objs
SHAREINCL		= $(SHAREDIR)/src/includes
#Threads library (POSIX threads)
LTHREAD			= -lpthread
#Files to remove with 'clear'/'clean'
TARGETS			= $(BIN)/* $(LIB_DIRS)/* $(OBJS)/*

#Header files
headers := $(wildcard $(INCLUDE_DIRS)/*.h) #Exercise-only
commons_h := $(wildcard $(SHAREINCL)/*.h) #Common interfaces
defines := $(wildcard $(SHARESRC)/*.h) #Common definitions
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDE_DIRS)/%.h=$(OBJS)/%.o)
commons_o := $(commons_h:$(SHAREINCL)/%.h=$(SHAREOBJS)/%.o)
#Executables that need static library
exelibs := es2
#All included files
commons_i := -I $(SHAREINCL) -I $(SHARESRC)
includes := -I $(INCLUDE_DIRS) $(commons_i)
#Dynamic linking path
dlpath := -Wl,-rpath,$(SHARELIB)



.PHONY: clear

.SUFFIXES: .c .h .o

all : es4.1 es4.2 es4.3

#Generals

#Generals

$(exelibs) : % : $(SRC)/%.c libutils.a libshared.so
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ $(LTHREAD) $(dlpath) -L $(LIBRARY) -L $(SHARELIB)/ -lshared

% : $(SRC)/%.c libshared.so
	$(CC) $(includes) $(CFLAGS) $< -o $(BIN)/$@ $(LTHREAD) $(dlpath) -L $(SHARELIB)/ -lshared

$(objects) : $(OBJS)/%.o : $(SRC)/%.c $(INCLUDE_DIRS)/%.h
	$(CC) $(includes) $(CFLAGS) -c $< -o $@ $(LTHREAD)

$(commons_o) : $(SHAREOBJS)/%.o : $(SHARESRC)/%.c $(SHAREINCL)/%.h $(defines)
	$(CC) -fPIC $(commons_i) $(CFLAGS) -c $< -o $@ $(LTHREAD)  

%.a : $(objects)
	ar rvs $(LIB_DIRS)/$@ $^

%.so : $(commons_o)
	$(CC) -shared -o $(SHARELIB)/$@ $^

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

