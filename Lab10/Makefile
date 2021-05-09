CC				=  gcc
CFLAGS			+= -Wall -g -pedantic -std=c99
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
TARGETS			= $(wildcard $(BIN)/*) $(wildcard $(LIB_DIRS)/*) $(wildcard $(OBJS)/*)
#Sockets to remove after an uncorrect termination
SOCKETS			= $(wildcard $(BIN)/socket*)


#Header files
headers := $(wildcard $(INCLUDE_DIRS)/*.h) #Exercise-only
commons_h := $(wildcard $(SHAREINCL)/*.h) #Common interfaces
defines := $(wildcard $(SHARESRC)/*.h) #Common definitions
#Object files (to be compiled from headers)
objects := $(headers:$(INCLUDE_DIRS)/%.h=$(OBJS)/%.o)
commons_o := $(commons_h:$(SHAREINCL)/%.h=$(SHAREOBJS)/%.o)
#Executables that need static library
#exelibs := mt_server_inia
#All included files
commons_i := -I $(SHAREINCL) -I $(SHARESRC)
includes := -I $(INCLUDE_DIRS) $(commons_i)
#Dynamic linking path
dlpath := -Wl,-rpath,$(SHARELIB)


.PHONY : clear
.PHONY : unlink
.PHONY : debug
.SUFFIXES : .c .h .o


all : client_poll es10.1 es10.2 es10.3

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

$(exelibs) : % : $(SRC)/%.c libshared.so libutils.a 
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
es10.1 : sigint_count

#Es2
es10.2 : mp_server_sigs

#Es3
es10.3 : mt_server_sigs

#Removes bin/objs/libs/sockets
clear :
	-rm $(TARGETS) 

#Removes sockets only
unlink :
	-rm $(SOCKETS)

