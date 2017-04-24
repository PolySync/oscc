project(kia-soul-can-gateway)

set(ARDUINO_DEFAULT_BOARD uno)

generate_arduino_firmware(
    kia-soul-can-gateway
    SKETCH src/main.ino
    SRCS ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp
    src/globals.cpp
    src/init.cpp
    src/communications.cpp)

target_include_directories(
    kia-soul-can-gateway
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)

add_subdirectory(tests)
