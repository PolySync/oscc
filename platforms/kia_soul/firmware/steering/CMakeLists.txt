project(kia-soul-steering)

set(ARDUINO_DEFAULT_BOARD uno)

generate_arduino_firmware(
    kia-soul-steering
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init/arduino_init.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/DAC_MCP49xx/DAC_MCP49xx.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/pid/pid.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp
    src/main.cpp
    src/globals.cpp
    src/init.cpp
    src/communications.cpp
    src/steering_control.cpp)

target_include_directories(
    kia-soul-steering
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/arduino_init
    ${CMAKE_SOURCE_DIR}/common/libs/DAC_MCP49xx
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)

add_subdirectory(tests)
