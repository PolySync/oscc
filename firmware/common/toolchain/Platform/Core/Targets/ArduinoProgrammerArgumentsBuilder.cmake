#=============================================================================#
# build_arduino_programmer_arguments
# [PRIVATE/INTERNAL]
#
# build_arduino_programmer_arguments(BOARD_ID PROGRAMMER TARGET_NAME PORT AVRDUDE_FLAGS OUTPUT_VAR)
#
#      BOARD_ID    - board id
#      PROGRAMMER  - programmer id
#      TARGET_NAME - target name
#      PORT        - serial port
#      AVRDUDE_FLAGS - avrdude flags (override)
#      OUTPUT_VAR  - name of output variable for result
#
# Sets up default avrdude settings for burning firmware via a programmer.
#=============================================================================#
function(build_arduino_programmer_arguments BOARD_ID PROGRAMMER TARGET_NAME PORT AVRDUDE_FLAGS OUTPUT_VAR)
    set(AVRDUDE_ARGS ${${OUTPUT_VAR}})

    if (NOT AVRDUDE_FLAGS)
        set(AVRDUDE_FLAGS ${ARDUINO_AVRDUDE_FLAGS})
    endif ()

    list(APPEND AVRDUDE_ARGS "-C${ARDUINO_AVRDUDE_CONFIG_PATH}")

    #TODO: Check mandatory settings before continuing
    if (NOT ${PROGRAMMER}.protocol)
        message(FATAL_ERROR "Missing ${PROGRAMMER}.protocol, aborting!")
    endif ()

    list(APPEND AVRDUDE_ARGS "-c${${PROGRAMMER}.protocol}") # Set programmer

    if (${PROGRAMMER}.communication STREQUAL "usb")
        list(APPEND AVRDUDE_ARGS "-Pusb") # Set USB as port
    elseif (${PROGRAMMER}.communication STREQUAL "serial")
        list(APPEND AVRDUDE_ARGS "-P${PORT}") # Set port
        if (${PROGRAMMER}.speed)
            list(APPEND AVRDUDE_ARGS "-b${${PROGRAMMER}.speed}") # Set baud rate
        endif ()
    endif ()

    if (${PROGRAMMER}.force)
        list(APPEND AVRDUDE_ARGS "-F") # Set force
    endif ()

    if (${PROGRAMMER}.delay)
        list(APPEND AVRDUDE_ARGS "-i${${PROGRAMMER}.delay}") # Set delay
    endif ()

    _get_board_property(${BOARD_ID} build.mcu MCU)
    list(APPEND AVRDUDE_ARGS "-p${MCU}")  # MCU Type

    list(APPEND AVRDUDE_ARGS ${AVRDUDE_FLAGS})

    set(${OUTPUT_VAR} ${AVRDUDE_ARGS} PARENT_SCOPE)
endfunction()
