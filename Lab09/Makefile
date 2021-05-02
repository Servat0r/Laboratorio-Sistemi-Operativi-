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
#Sockets to remove after an uncorrect termination
SOCKETS			= $(BIN)/socket*


#Header files
headers := $(wildcard $(INCLUDE_DIRS)/*.h) #Exercise-only
commons_h := $(wildcard $(SHAREINCL)/*.h) #Common interfaces
defines := $(wildcard $(SHARESRC)/*.h) #Common definitions
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDE_DIRS)/%.h=$(OBJS)/%.o)
commons_o := $(commons_h:$(SHAREINCL)/%.h=$(SHAREOBJS)/%.o)
#Executables that need static library
exelibs := test_tsqueue multiple_pc
#All included files
commons_i := -I $(SHAREINCL) -I $(SHARESRC)
includes := -I $(INCLUDE_DIRS) $(commons_i)
#Dynamic linking path
dlpath := -Wl,-rpath,$(SHARELIB)


.PHONY : clear
.PHONY : unlink
.PHONY : debug
.SUFFIXES : .c .h .o


all : es9.1 es9.2 es9.3

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

#Es1
es9.1 : client client_poll sc_server sc_server_direct

#Es2
es9.2 : client client_poll mt_server

#Es3
es9.3 : client client_poll mp_server mp_server_term

#Removes bin/objs/libs/sockets
clear :
	-rm $(TARGETS) 

#Removes sockets only
unlink :
	-rm $(SOCKETS)

