include(ArduinoBootloaderUploadTargetCreator)

#=============================================================================#
# create_arduino_upload_target
# [PRIVATE/INTERNAL]
#
# create_arduino_upload_target(BOARD_ID TARGET_NAME PORT)
#
#        BOARD_ID    - Arduino board id
#        TARGET_NAME - Target name
#        PORT        - Serial port for upload
#        PROGRAMMER_ID - Programmer ID
#        AVRDUDE_FLAGS - avrdude flags
#
# Create an upload target (${TARGET_NAME}-upload) for the specified Arduino target.
#
#=============================================================================#
function(create_arduino_upload_target BOARD_ID TARGET_NAME PORT PROGRAMMER_ID AVRDUDE_FLAGS)
    create_arduino_bootloader_upload_target(${TARGET_NAME} ${BOARD_ID} ${PORT} "${AVRDUDE_FLAGS}")

    # Add programmer support if defined
    if (PROGRAMMER_ID AND ${PROGRAMMER_ID}.protocol)
        create_arduino_programmer_burn_target(${TARGET_NAME} ${BOARD_ID} ${PROGRAMMER_ID} ${PORT} "${AVRDUDE_FLAGS}")
        create_arduino_bootloader_burn_target(${TARGET_NAME} ${BOARD_ID} ${PROGRAMMER_ID} ${PORT} "${AVRDUDE_FLAGS}")
    endif ()
endfunction()
