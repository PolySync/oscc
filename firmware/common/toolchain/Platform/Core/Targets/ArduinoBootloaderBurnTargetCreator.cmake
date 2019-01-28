#=============================================================================#
# create_arduino_bootloader_burn_target
# [PRIVATE/INTERNAL]
#
# create_arduino_bootloader_burn_target(TARGET_NAME BOARD_ID PROGRAMMER PORT AVRDUDE_FLAGS)
#
#      TARGET_NAME - name of target to burn
#      BOARD_ID    - board id
#      PROGRAMMER  - programmer id
#      PORT        - serial port
#      AVRDUDE_FLAGS - avrdude flags (override)
#
# Create a target for burning a bootloader via a programmer.
#
# The target for burning the bootloader is ${TARGET_NAME}-burn-bootloader
#
#=============================================================================#
function(create_arduino_bootloader_burn_target TARGET_NAME BOARD_ID PROGRAMMER PORT AVRDUDE_FLAGS)
    set(BOOTLOADER_TARGET ${TARGET_NAME}-burn-bootloader)

    set(AVRDUDE_ARGS)

    build_arduino_programmer_arguments(${BOARD_ID} ${PROGRAMMER} ${TARGET_NAME} ${PORT} "${AVRDUDE_FLAGS}" AVRDUDE_ARGS)

    if (NOT AVRDUDE_ARGS)
        message("Could not generate default avrdude programmer args, aborting!")
        return()
    endif ()

    # look at bootloader.file
    _try_get_board_property(${BOARD_ID} bootloader.file BOOTLOADER_FILE)
    if (NOT BOOTLOADER_FILE)
        message("Missing bootloader.file, not creating bootloader burn target ${BOOTLOADER_TARGET}.")
        return()
    endif()

    # build bootloader.path from bootloader.file...
    string(REGEX MATCH "(.+/)*" BOOTLOADER_PATH ${BOOTLOADER_FILE})
    string(REGEX REPLACE "/" "" BOOTLOADER_PATH ${BOOTLOADER_PATH})
    # and fix bootloader.file
    string(REGEX MATCH "/.(.+)$" BOOTLOADER_FILE_NAME ${BOOTLOADER_FILE})
    string(REGEX REPLACE "/" "" BOOTLOADER_FILE_NAME ${BOOTLOADER_FILE_NAME})

    if (NOT EXISTS "${ARDUINO_BOOTLOADERS_PATH}/${BOOTLOADER_PATH}/${BOOTLOADER_FILE_NAME}")
        message("Missing bootloader image '${ARDUINO_BOOTLOADERS_PATH}/${BOOTLOADER_PATH}/${BOOTLOADER_FILE_NAME}', not creating bootloader burn target ${BOOTLOADER_TARGET}.")
        return()
    endif ()

    #check for required bootloader parameters
    foreach (ITEM lock_bits unlock_bits high_fuses low_fuses)
        #do not make fatal error if field doesn't exists, just don't create bootloader burn target
        _try_get_board_property(${BOARD_ID} bootloader.${ITEM} BOOTLOADER_${ITEM})
        if (NOT BOOTLOADER_${ITEM})
            message("Missing bootloader.${ITEM}, not creating bootloader burn target ${BOOTLOADER_TARGET}.")
            return()
        endif ()
    endforeach ()

    # Erase the chip
    list(APPEND AVRDUDE_ARGS "-e")

    # Set unlock bits and fuses (because chip is going to be erased)
    list(APPEND AVRDUDE_ARGS "-Ulock:w:${BOOTLOADER_unlock_bits}:m")
    # extended fuses is optional
    _try_get_board_property(${BOARD_ID} bootloader.extended_fuses BOOTLOADER_extended_fuses)
    if (BOOTLOADER_extended_fuses)
        list(APPEND AVRDUDE_ARGS "-Uefuse:w:${BOOTLOADER_extended_fuses}:m")
    endif()

    list(APPEND AVRDUDE_ARGS
            "-Uhfuse:w:${BOOTLOADER_high_fuses}:m"
            "-Ulfuse:w:${BOOTLOADER_low_fuses}:m"
            "-Uflash:w:${BOOTLOADER_FILE_NAME}:i"
            "-Ulock:w:${BOOTLOADER_lock_bits}:m")

    # Create burn bootloader target
    add_custom_target(${BOOTLOADER_TARGET}
            ${ARDUINO_AVRDUDE_PROGRAM}
            ${AVRDUDE_ARGS}
            WORKING_DIRECTORY ${ARDUINO_BOOTLOADERS_PATH}/${BOOTLOADER_PATH}
            DEPENDS ${TARGET_NAME})
endfunction()
