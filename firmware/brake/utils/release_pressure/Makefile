# The MIT License (MIT)
# =====================

# Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved.

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
# Makefile for Brake Actuator Module
##########################################################

#
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    OSFLAG = LINUX
endif
ifeq ($(UNAME_S),Darwin)
    OSFLAG = OSX
endif

#
PROJECT_DIR := $(shell pwd)

#
ARDMK_DIR = $(PROJECT_DIR)/../../../arduino_make

ifeq ($(OSFLAG), LINUX)
    ARDUINO_DIR = /usr/share/arduino

    BOARD_TAG = mega2560

    AVRDUDE = /usr/bin/avrdude

    MONITOR_PORT = /dev/ttyACM*
endif
ifeq ($(OSFLAG), OSX)
    ARDUINO_DIR=/Applications/Arduino.app/Contents/Java/

    BOARD_TAG = mega
    BOARD_SUB = atmega2560

    AVRDUDE = /usr/local/bin/avrdude

    MONITOR_PORT = /dev/cu.usb*
endif

#
USER_LIB_PATH = $(PROJECT_DIR)/../../../arduino_libraries

#
MONITOR_BAUDRATE = 115200

#
AVR_TOOLS_DIR =

#
CFLAGS_STD = -std=gnu11

#
CXXFLAGS_STD = -std=gnu++11

#
#CXXFLAGS += -pedantic -Wall -Wextra
CXXFLAGS += -Wall -Wextra

#
CFLAGS_STD += -DPSYNC_DEBUG_FLAG
#CXXFLAGS += -DPSYNC_DEBUG_FLAG


#
CURRENT_DIR = $(shell basename $(CURDIR))

#
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)

#
include $(ARDMK_DIR)/Arduino.mk
