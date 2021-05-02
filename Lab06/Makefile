CC				=  gcc
CFLAGS			+= -Wall -g -pedantic  #-std=c99
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
commons_o := $(commons_h:$(SHAREINCL)/%.h=$(SHAREOBJS)/%.o)#Executables that need library
exelibs := dummyshell
#All included files
commons_i := -I $(SHAREINCL) -I $(SHARESRC)
includes := -I $(INCLUDE_DIRS) $(commons_i)
#Dynamic linking path
dlpath := -Wl,-rpath,$(SHARELIB)


.PHONY : clear
.PHONY : debug
.SUFFIXES : .c .h .o

all : es6.1 es6.2 es6.3 es6.4 es6.5

debug :
	@echo headers = $(headers)
	@echo commons_h = $(commons_h)
	@echo commons_o = $(commons_o)
	@echo defines = $(defines)
	@echo objects = $(objects)
	@echo includes = $(includes)
	@echo library = $(LIBRARY)
	@echo exelibs = $(exelibs)

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
