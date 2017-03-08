##########################################################
# Makefile for Brake Actuator Module
##########################################################

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

#
ifeq ($(OSFLAG), LINUX)
    ARDUINO_DIR = /usr/share/arduino

    BOARD_TAG = mega2560

    AVRDUDE = /usr/bin/avrdude
endif
ifeq ($(OSFLAG), OSX)
    ARDUINO_DIR=/Applications/Arduino.app/Contents/Java/
    
    BOARD_TAG = mega
    BOARD_SUB = atmega2560

    AVRDUDE = /usr/local/bin/avrdude
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
ifeq ($(OSFLAG), LINUX)
    MONITOR_PORT = /dev/ttyACM*
endif
ifeq ($(OSFLAG), OSX)
    MONITOR_PORT = /dev/tty.usb*
endif

#
CURRENT_DIR = $(shell basename $(CURDIR))

#
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)

#
include $(ARDMK_DIR)/Arduino.mk
