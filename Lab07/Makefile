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
commons_o := $(commons_h:$(SHAREINCL)/%.h=$(SHAREOBJS)/%.o)
#Executables that need static library
exelibs := philosophers_2
#All included files
commons_i := -I $(SHAREINCL) -I $(SHARESRC)
includes := -I $(INCLUDE_DIRS) $(commons_i)
#Dynamic linking path
dlpath := -Wl,-rpath,$(SHARELIB)


.PHONY : clear
.PHONY : debug
.SUFFIXES : .c .h .o


all : es7.1 es7.2 es7.3 es7.4

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


#Ex1
es7.1 : libutils.a pc_arraystack pc_arraystack_for pc_boundedqueue

#Ex2
es7.2 : libutils.a philosophers philosophers_2

#Ex3
es7.3 : libutils.a pipeline3

#Ex4
es7.4 : libutils.a pipeline4

clear :
	-rm $(TARGETS)

