project(brake)

set(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_BRAKE})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_BRAKE})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

if(${VEHICLE} STREQUAL "kia_soul")
    add_subdirectory(kia_soul_petrol)
else()
    add_subdirectory(kia_soul_ev_niro)
endif()
