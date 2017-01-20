# The MIT License (MIT)
# =====================

# Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved.

# Permission is hereby granted, free of charge, to any person
# obtaining a copy of this software and associated documentation
# files (the “Software”), to deal in the Software without
# restriction, including without limitation the rights to use,
# copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following
# conditions:

# The above copyright notice and this permission notice shall be
# included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
# HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
# WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.

##########################################################
# Makefile for Joystick Commander
##########################################################


LIB_HOME = ../../firmware/arduino_libraries/control_protocol_can

# target
TARGET  := bin/joystick-commander

# source files
SRCS    :=  src/joystick.c \
	    src/messages.c \
	    src/commander.c \
	    src/node.c \
	    src/can_read.c

# dep files
OBJS    := $(SRCS:.c=.o)
DEPS    := $(SRCS:.c=.dep)
XDEPS   := $(wildcard $(DEPS))

# compiler
CC = gcc

# cflags
CCFLAGS += `pkg-config --cflags sdl2`

# include directories
INCLUDE += -Iinclude -I$(LIB_HOME)

# add node template library, must be first
# add util library last, let path be found
LIBS := $(LIBS) `pkg-config --libs sdl2` -lcanlib -lm

# all builds directories and target
all: dirs $(TARGET)

# check deps
ifneq ($(XDEPS),)
include $(XDEPS)
endif

# target
$(TARGET): $(OBJS)
	$(CC) $(LDFLAGS) $(INCLUDE) -o $@ $^ $(LIBS)

# object files
$(OBJS): %.o: %.c %.dep
	$(CC) $(CCFLAGS) $(INCLUDE) -o $@ -c $<

# dep files
$(DEPS): %.dep: %.c Makefile
	$(CC) $(CCFLAGS) $(INCLUDE) -MM $< > $@

# directories
dirs::
	mkdir -p bin
	mkdir -p doc/html

# clean
clean::
	rm -f src/*.o
	rm -f src/*.dep
	rm -f $(TARGET)
	rm -rf bin
	rm -rf doc/html
	rm -rf doc/latex
	rm -rf doc/pdf
	rm -f ospl-*.log
