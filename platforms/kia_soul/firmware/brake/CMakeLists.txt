project(kia-soul-brake)

set(ARDUINO_DEFAULT_BOARD mega2560)
SET(ARDUINO_DEFAULT_PORT ${SERIAL_PORT_BRAKE})
set(ARDUINO_DEFAULT_BAUDRATE ${SERIAL_BAUD_BRAKE})

add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    kia-soul-brake-monitor
    COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

add_custom_target(
    kia-soul-brake-monitor-log
    COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

generate_arduino_firmware(
    kia-soul-brake
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init/arduino_init.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/pid/oscc_pid.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/serial/oscc_serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing/oscc_signal_smoothing.cpp
    src/main.cpp
    src/globals.cpp
    src/accumulator.cpp
    src/helper.cpp
    src/master_cylinder.cpp
    src/brake_control.cpp
    src/communications.cpp
    src/init.cpp)

target_include_directories(
    kia-soul-brake
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing)

add_subdirectory(utils)
