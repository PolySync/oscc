#=============================================================================#
# create_arduino_target
# [PRIVATE/INTERNAL]
#
# create_arduino_target(TARGET_NAME ALL_SRCS ALL_LIBS COMPILE_FLAGS LINK_FLAGS MANUAL)
#
#        TARGET_NAME - Target name
#        BOARD_ID    - Arduino board ID
#        ALL_SRCS    - All sources
#        ALL_LIBS    - All libraries
#        COMPILE_FLAGS - Compile flags
#        LINK_FLAGS    - Linker flags
#        MANUAL - (Advanced) Only use AVR Libc/Includes
#
# Creates an Arduino firmware target.
#
#=============================================================================#
function(create_arduino_firmware_target TARGET_NAME BOARD_ID ALL_SRCS ALL_LIBS
        COMPILE_FLAGS LINK_FLAGS MANUAL)

    string(STRIP "${ALL_SRCS}" ALL_SRCS)
    add_executable(${TARGET_NAME} "${ALL_SRCS}")
    set_target_properties(${TARGET_NAME} PROPERTIES SUFFIX ".elf")

    set_board_flags(ARDUINO_COMPILE_FLAGS ARDUINO_LINK_FLAGS ${BOARD_ID} ${MANUAL})

    set_target_properties(${TARGET_NAME} PROPERTIES
            COMPILE_FLAGS "${ARDUINO_COMPILE_FLAGS} ${COMPILE_FLAGS}"
            LINK_FLAGS "${ARDUINO_LINK_FLAGS} ${LINK_FLAGS}")
    target_link_libraries(${TARGET_NAME} ${ALL_LIBS} "-lc -lm")

    if (NOT EXECUTABLE_OUTPUT_PATH)
        set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
    endif ()
    set(TARGET_PATH ${EXECUTABLE_OUTPUT_PATH}/${TARGET_NAME})
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY}
            ARGS ${ARDUINO_OBJCOPY_EEP_FLAGS}
            ${TARGET_PATH}.elf
            ${TARGET_PATH}.eep
            COMMENT "Generating EEP image"
            VERBATIM)

    # Convert firmware image to ASCII HEX format
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY}
            ARGS ${ARDUINO_OBJCOPY_HEX_FLAGS}
            ${TARGET_PATH}.elf
            ${TARGET_PATH}.hex
            COMMENT "Generating HEX image"
            VERBATIM)
    _get_board_property(${BOARD_ID} build.mcu MCU)
    # Display target size
    add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -DFIRMWARE_IMAGE=${TARGET_PATH}.elf
            -DMCU=${MCU}
            -DEEPROM_IMAGE=${TARGET_PATH}.eep
            -P ${ARDUINO_SIZE_SCRIPT}
            COMMENT "Calculating image size"
            VERBATIM)

    # Create ${TARGET_NAME}-size target
    add_custom_target(${TARGET_NAME}-size
            COMMAND ${CMAKE_COMMAND}
            -DFIRMWARE_IMAGE=${TARGET_PATH}.elf
            -DMCU=${MCU}
            -DEEPROM_IMAGE=${TARGET_PATH}.eep
            -P ${ARDUINO_SIZE_SCRIPT}
            DEPENDS ${TARGET_NAME}
            COMMENT "Calculating ${TARGET_NAME} image size")

endfunction()
