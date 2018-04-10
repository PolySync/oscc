project(null)

SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_NULL})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_NULL})

generate_arduino_firmware(
    null
    SRCS
    main.cpp)
