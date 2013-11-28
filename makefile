#################################################
# MAKEFILE for 3d object drawing on RaspBerry	#
# (c) 20131115 Clemente di Caprio				#
#                                               #
#################################################

CC=gcc

#
# Compiler options
OPTIMIZE=-O3
GDBDEBUG=-g
CFLAGS=-c -Wall
CFLAGS+=$(OPTIMIZE)
CFLAGS+=$(GDBDEBUG)

#
# Library 
MATH_LIB=-lm
TOUCH_LIB=-lts-0.0
THREAD_LIB=-lpthread
CLKGETTIME_LIB=-lrt
#
LDFLAGS=$(MATH_LIB)
LDFLAGS+=$(TOUCH_LIB)
LDFLAGS+=$(THREAD_LIB)
LDFLAGS+=$(CLKGETTIME_LIB)

#
# Source path
TOUCH_FILE=./lib/touch
FBLCD_FILE=./lib/fblcd
#
# Include
INCPATHS=./inc				\
		$(TOUCH_FILE)/inc 	\
		$(FBLCD_FILE)/inc
INCLUDES     = $(addprefix -I ,$(INCPATHS))

#
SOURCES=./src/main.c 					\
		./src/3d.c 						\
		./src/3dapp.c 					\
		./src/drawobj.c 				\
		$(FBLCD_FILE)/src/drv_glcd.c 	\
		$(TOUCH_FILE)/src/touchscreen.c
		
OBJECTS=$(SOURCES:%.c=%.o)
EXECUTABLE=3dobj

MSGECHO = echo
APP_VER = W.I.P
REMOVAL = rm
REMOVE	= $(REMOVAL) -f

all: gccversion build 
build: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS)

.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@

clean:
	$(REMOVE) $(EXECUTABLE)
	$(REMOVE) *.o
	$(REMOVE) $(wildcard $(TOUCH_FILE)/src/*.o)
	$(REMOVE) $(wildcard $(FBLCD_FILE)/src/*.o)
	$(REMOVE) $(wildcard ./src/*.o)

gccversion : 
	@$(CC) --version
	@$(MSGECHO) 

.PHONY : all gccversion
