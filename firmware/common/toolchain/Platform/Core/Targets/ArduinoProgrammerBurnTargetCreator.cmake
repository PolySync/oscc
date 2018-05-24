#=============================================================================#
# create_arduino_programmer_burn_target
# [PRIVATE/INTERNAL]
#
# create_arduino_programmer_burn_target(TARGET_NAME BOARD_ID PROGRAMMER PORT AVRDUDE_FLAGS)
#
#      TARGET_NAME - name of target to burn
#      BOARD_ID    - board id
#      PROGRAMMER  - programmer id
#      PORT        - serial port
#      AVRDUDE_FLAGS - avrdude flags (override)
#
# Sets up target for burning firmware via a programmer.
#
# The target for burning the firmware is ${TARGET_NAME}-burn .
#
#=============================================================================#
function(create_arduino_programmer_burn_target TARGET_NAME BOARD_ID PROGRAMMER PORT AVRDUDE_FLAGS)
    set(PROGRAMMER_TARGET ${TARGET_NAME}-burn)

    set(AVRDUDE_ARGS)

    build_arduino_programmer_arguments(${BOARD_ID} ${PROGRAMMER} ${TARGET_NAME} ${PORT} "${AVRDUDE_FLAGS}" AVRDUDE_ARGS)

    if (NOT AVRDUDE_ARGS)
        message("Could not generate default avrdude programmer args, aborting!")
        return()
    endif ()

    if (NOT EXECUTABLE_OUTPUT_PATH)
        set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
    set(TARGET_PATH ${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME})

    list(APPEND AVRDUDE_ARGS "-Uflash:w:\"${TARGET_PATH}.hex\":i")

    add_custom_target(${PROGRAMMER_TARGET}
            ${ARDUINO_AVRDUDE_PROGRAM}
            ${AVRDUDE_ARGS}
            DEPENDS ${TARGET_NAME})
endfunction()
