project(brake)

SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_BRAKE})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_BRAKE})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

if(KIA_SOUL)
    add_subdirectory(kia_soul_petrol)
elseif(KIA_SOUL_EV)
    add_subdirectory(kia_soul_ev)
endif()
