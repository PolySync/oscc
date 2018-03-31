project(can-gateway)

SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_CAN_GATEWAY})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_CAN_GATEWAY})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    can-gateway-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    can-gateway-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

generate_arduino_firmware(
    can-gateway
    SRCS
    ${OSCC_FIRMWARE_ROOT}/common/libs/arduino_init/arduino_init.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/mcp_can/mcp_can.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/serial/oscc_serial.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/can/oscc_can.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/timer/oscc_timer.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/ssd1325/ssd1325.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/ssd1325/gfx/gfx.cpp
    src/main.cpp
    src/globals.cpp
    src/init.cpp
    src/communications.cpp
    src/display.cpp
    src/timer.cpp)

target_include_directories(
    can-gateway
    PRIVATE
    include
    ${OSCC_FIRMWARE_ROOT}/common/include
    ${OSCC_FIRMWARE_ROOT}/common/libs/arduino_init
    ${OSCC_FIRMWARE_ROOT}/common/libs/mcp_can
    ${OSCC_FIRMWARE_ROOT}/common/libs/serial
    ${OSCC_FIRMWARE_ROOT}/common/libs/can
    ${OSCC_FIRMWARE_ROOT}/common/libs/timer
    ${OSCC_FIRMWARE_ROOT}/common/libs/ssd1325
    ${OSCC_FIRMWARE_ROOT}/common/libs/ssd1325/gfx
    ${OSCC_FIRMWARE_ROOT}/../api/include)
