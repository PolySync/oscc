#=============================================================================#
# create_serial_target
# [PRIVATE/INTERNAL]
#
# create_serial_target(TARGET_NAME CMD)
#
#         TARGET_NAME - Target name
#         CMD         - Serial terminal command
#
# Creates a target (${TARGET_NAME}-serial) for launching the serial termnial.
#
#=============================================================================#
function(create_serial_target TARGET_NAME CMD SERIAL_PORT)
    string(CONFIGURE "${CMD}" FULL_CMD @ONLY)
    add_custom_target(${TARGET_NAME}-serial
            COMMAND ${FULL_CMD})
endfunction()
