#=============================================================================#
# Original Author: Tomasz Bogdal (QueezyTheGreat)
# Current Author: Timor Gruber (MrPointer)
# Original Home: https://github.com/queezythegreat/arduino-cmake
# Current Home: https://github.com/arduino-cmake/arduino-cmake
#
# This Source Code Form is subject to the terms of the Mozilla Public
# License, v. 2.0. If a copy of the MPL was not distributed with this file,
# You can obtain one at http://mozilla.org/MPL/2.0/.
#=============================================================================#

cmake_minimum_required(VERSION 2.8.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Initialization)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core/BoardFlags)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core/Libraries)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core/Targets)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core/Sketch)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Core/Examples)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Extras)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/Generation)

include(CMakeParseArguments)

include(VariableValidator)
include(Initializer)

include(Macros)
include(BoardPropertiesReader)
include(FlagsSetter)
include(SourceFinder)
include(LibraryFinder)
include(DebugOptions)
include(Printer)
include(GeneratorSettingsLoader)

include(ArduinoSketchToCppConverter)
include(ArduinoSketchFactory)

include(CoreLibraryFactory)
include(ArduinoLibraryFactory)
include(BlacklistedLibrariesRemover)

include(ArduinoExampleFactory)
include(ArduinoLibraryExampleFactory)

include(ArduinoBootloaderArgumentsBuilder)
include(ArduinoBootloaderBurnTargetCreator)
include(ArduinoBootloaderUploadTargetCreator)
include(ArduinoFirmwareTargetCreator)
include(ArduinoProgrammerArgumentsBuilder)
include(ArduinoProgrammerBurnTargetCreator)
include(ArduinoSerialTargetCreator)
include(ArduinoUploadTargetCreator)

include(AvrLibraryGenerator)
include(AvrFirmwareGenerator)
include(ArduinoLibraryGenerator)
include(ArduinoFirmwareGenerator)
include(ArduinoExampleGenerator)
include(ArduinoLibraryExampleGenerator)

if (IS_SCRIPT_PROCESSED)
    return()
endif ()

# Define the 'RECURSE' flag for libraries known to depend on it
# on each reload
set(Wire_RECURSE True)
set(Ethernet_RECURSE True)
set(SD_RECURSE True)

set(IS_SCRIPT_PROCESSED True)
