##########################################################
# Makefile for Vehicle Gateway Module
##########################################################

#
PROJECT_DIR := $(shell pwd)

#
ARDMK_DIR = $(PROJECT_DIR)/../../arduino_make

#
ARDUINO_DIR = /usr/share/arduino

#
USER_LIB_PATH = $(PROJECT_DIR)/../../arduino_libraries

#
BOARD_TAG = uno

#
MONITOR_BAUDRATE = 115200

#
AVR_TOOLS_DIR =

#
AVRDUDE = /usr/bin/avrdude

#
CFLAGS_STD = -std=gnu11

#
CXXFLAGS_STD = -std=gnu++11

#
CXXFLAGS += -pedantic -Wall -Wextra

#
CFLAGS_STD += -DPSYNC_DEBUG_FLAG
CXXFLAGS += -DPSYNC_DEBUG_FLAG

#
MONITOR_PORT = /dev/ttyACM*

#
CURRENT_DIR = $(shell basename $(CURDIR))

#
OBJDIR = $(PROJECT_DIR)/bin/$(BOARD_TAG)/$(CURRENT_DIR)

#
include $(ARDMK_DIR)/Arduino.mk
