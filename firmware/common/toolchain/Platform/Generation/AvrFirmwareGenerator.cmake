#=============================================================================#
# GENERATE_AVR_FIRMWARE
# [PUBLIC/USER]
# see documentation at README
#=============================================================================#
function(GENERATE_AVR_FIRMWARE INPUT_NAME)
    # TODO: This is not optimal!!!!
    message(STATUS "Generating ${INPUT_NAME}")
    parse_generator_arguments(${INPUT_NAME} INPUT
            "NO_AUTOLIBS;MANUAL"               # Options
            "BOARD;BOARD_CPU;PORT;PROGRAMMER"  # One Value Keywords
            "SERIAL;SRCS;HDRS;LIBS;AFLAGS"     # Multi Value Keywords
            ${ARGN})

    if (NOT INPUT_BOARD)
        set(INPUT_BOARD ${ARDUINO_DEFAULT_BOARD})
    endif ()
    if (NOT INPUT_PORT)
        set(INPUT_PORT ${ARDUINO_DEFAULT_PORT})
    endif ()
    if (NOT INPUT_SERIAL)
        set(INPUT_SERIAL ${ARDUINO_DEFAULT_SERIAL})
    endif ()
    if (NOT INPUT_PROGRAMMER)
        set(INPUT_PROGRAMMER ${ARDUINO_DEFAULT_PROGRAMMER})
    endif ()

    VALIDATE_VARIABLES_NOT_EMPTY(VARS INPUT_BOARD INPUT_SRCS MSG "must define for target ${INPUT_NAME}")

    if (INPUT_HDRS)
        list(INSERT INPUT_HDRS 0 "HDRS")
    endif ()
    if (INPUT_LIBS)
        list(INSERT INPUT_LIBS 0 "LIBS")
    endif ()
    if (INPUT_AFLAGS)
        list(INSERT INPUT_AFLAGS 0 "AFLAGS")
    endif ()

    generate_arduino_firmware(${INPUT_NAME}
            NO_AUTOLIBS
            MANUAL
            BOARD ${INPUT_BOARD}
            BOARD_CPU ${INPUT_BOARD_CPU}
            PORT ${INPUT_PORT}
            PROGRAMMER ${INPUT_PROGRAMMER}
            SERIAL ${INPUT_SERIAL}
            SRCS ${INPUT_SRCS}
            ${INPUT_HDRS}
            ${INPUT_LIBS}
            ${INPUT_AFLAGS})

endfunction()
