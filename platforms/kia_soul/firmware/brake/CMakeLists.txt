project(kia-soul-brake)

set(ARDUINO_DEFAULT_BOARD mega2560)

generate_arduino_firmware(
    kia-soul-brake
    SKETCH src/main.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/PID/PID.c
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp
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
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/PID
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)


generate_arduino_firmware(
    kia-soul-brake-test-controller
    SKETCH tests/brake_controller_r0_test.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/PID/PID.c
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp
    src/globals.cpp
    src/accumulator.cpp
    src/helper.cpp
    src/master_cylinder.cpp
    src/brake_control.cpp
    src/communications.cpp
    src/init.cpp)

target_include_directories(
    kia-soul-brake-test-controller
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/PID
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)

add_subdirectory(utils)
