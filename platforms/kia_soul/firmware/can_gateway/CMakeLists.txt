project(kia-soul-can-gateway)

set(ARDUINO_DEFAULT_BOARD uno)
SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_CAN_GATEWAY})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_CAN_GATEWAY})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    kia-soul-can-gateway-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    kia-soul-can-gateway-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

generate_arduino_firmware(
    kia-soul-can-gateway
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init/arduino_init.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/serial/oscc_serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    src/main.cpp
    src/globals.cpp
    src/init.cpp
    src/communications.cpp)

target_include_directories(
    kia-soul-can-gateway
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)
