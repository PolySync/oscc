# Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved.

# This file is part of Open Source Car Control (OSCC).

# OSCC is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.

# OSCC is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with OSCC.  If not, see <http://www.gnu.org/licenses/>.

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
	    src/node.c

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
LIBS := $(LIBS) `pkg-config --libs sdl2` -lcanlib

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
