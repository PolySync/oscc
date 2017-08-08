project(brake)

set(ARDUINO_DEFAULT_BOARD mega2560)
SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_BRAKE})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_BRAKE})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    brake-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

if(KIA_SOUL_PETROL)
    add_subdirectory(kia_soul_petrol)
endif()
