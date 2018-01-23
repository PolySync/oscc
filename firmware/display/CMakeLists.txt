project(display)

if(DRIVEKIT)
    set(ARDUINO_DEFAULT_BOARD leonardo)
elseif(OSCC)
    set(ARDUINO_DEFAULT_BOARD uno)
endif()

SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_CAN_GATEWAY})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_CAN_GATEWAY})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    display-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    display-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

generate_arduino_firmware(
    display
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init/arduino_init.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/serial/oscc_serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/timer/oscc_timer.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/ssd1325/ssd1325.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/ssd1325/gfx/gfx.cpp
    src/main.cpp
    src/globals.cpp
    src/init.cpp
    src/communications.cpp
    src/display.cpp
    src/timer.cpp)

target_include_directories(
    display
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/timer
    ${CMAKE_SOURCE_DIR}/common/libs/ssd1325
    ${CMAKE_SOURCE_DIR}/common/libs/ssd1325/gfx
    ${CMAKE_SOURCE_DIR}/../api/include)
