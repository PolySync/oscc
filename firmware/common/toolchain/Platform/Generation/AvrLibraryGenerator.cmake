#=============================================================================#
# GENERATE_AVR_LIBRARY
# [PUBLIC/USER]
# see documentation at README
#=============================================================================#
function(GENERATE_AVR_LIBRARY INPUT_NAME)
    message(STATUS "Generating ${INPUT_NAME}")
    parse_generator_arguments(${INPUT_NAME} INPUT
            "NO_AUTOLIBS;MANUAL"                  # Options
            "BOARD;BOARD_CPU"                     # One Value Keywords
            "SRCS;HDRS;LIBS"                      # Multi Value Keywords
            ${ARGN})

    if (NOT INPUT_BOARD)
        set(INPUT_BOARD ${ARDUINO_DEFAULT_BOARD})
    endif ()

    VALIDATE_VARIABLES_NOT_EMPTY(VARS INPUT_SRCS INPUT_BOARD MSG "must define for target ${INPUT_NAME}")

    if (INPUT_HDRS)
        set(INPUT_HDRS "SRCS ${INPUT_HDRS}")
    endif ()
    if (INPUT_LIBS)
        set(INPUT_LIBS "LIBS ${INPUT_LIBS}")
    endif ()

    if (INPUT_HDRS)
        list(INSERT INPUT_HDRS 0 "HDRS")
    endif ()
    if (INPUT_LIBS)
        list(INSERT INPUT_LIBS 0 "LIBS")
    endif ()


    generate_arduino_library(${INPUT_NAME}
            NO_AUTOLIBS
            MANUAL
            BOARD ${INPUT_BOARD}
            BOARD_CPU ${INPUT_BOARD_CPU}
            SRCS ${INPUT_SRCS}
            ${INPUT_HDRS}
            ${INPUT_LIBS})

endfunction()
