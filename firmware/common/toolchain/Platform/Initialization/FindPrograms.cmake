find_file(ARDUINO_EXAMPLES_PATH
        NAMES examples
        PATHS ${ARDUINO_SDK_PATH}
        DOC "Path to directory containg the Arduino built-in examples."
        NO_DEFAULT_PATH)

find_file(ARDUINO_LIBRARIES_PATH
        NAMES libraries
        PATHS ${ARDUINO_SDK_PATH}
        DOC "Path to directory containing the Arduino libraries."
        NO_DEFAULT_PATH)

find_program(ARDUINO_AVRDUDE_PROGRAM
        NAMES avrdude
        PATHS ${ARDUINO_SDK_PATH}
        PATH_SUFFIXES hardware/tools hardware/tools/avr/bin
        NO_DEFAULT_PATH)

find_program(ARDUINO_AVRDUDE_PROGRAM
        NAMES avrdude
        DOC "Path to avrdude programmer binary.")

find_program(AVRSIZE_PROGRAM
        NAMES avr-size)

find_file(ARDUINO_AVRDUDE_CONFIG_PATH
        NAMES avrdude.conf
        PATHS ${ARDUINO_SDK_PATH} /etc/avrdude /etc
        PATH_SUFFIXES hardware/tools
        hardware/tools/avr/etc
        DOC "Path to avrdude programmer configuration file.")

if (ARDUINO_SDK_VERSION VERSION_LESS 1.0.0)
    find_file(ARDUINO_PLATFORM_HEADER_FILE_PATH
            NAMES WProgram.h
            PATHS ${ARDUINO_SDK_PATH}
            PATH_SUFFIXES hardware/arduino/avr/cores/arduino
            DOC "Path to Arduino platform's main header file"
            NO_DEFAULT_PATH)
else ()
    find_file(ARDUINO_PLATFORM_HEADER_FILE_PATH
            NAMES Arduino.h
            PATHS ${ARDUINO_SDK_PATH}
            PATH_SUFFIXES hardware/arduino/avr/cores/arduino
            DOC "Path to Arduino platform's main header file"
            NO_DEFAULT_PATH)
endif ()

if (NOT CMAKE_OBJCOPY)
    find_program(AVROBJCOPY_PROGRAM
            avr-objcopy)
    set(ADDITIONAL_REQUIRED_VARS AVROBJCOPY_PROGRAM)
    set(CMAKE_OBJCOPY ${AVROBJCOPY_PROGRAM})
endif (NOT CMAKE_OBJCOPY)

if (EXISTS "${ARDUINO_EXAMPLES_PATH}")
    include(SetupExampleCategories)
endif ()
