project(kia-soul-brake)

set(ARDUINO_DEFAULT_BOARD mega2560)

generate_arduino_firmware(
    kia-soul-brake
    SKETCH src/brake_control_module.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/FiniteStateMachine/FiniteStateMachine.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/PID/PID.c
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp)

target_include_directories(
    kia-soul-brake
    PRIVATE
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/FiniteStateMachine
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/PID
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)

add_subdirectory(tests)
add_subdirectory(utils)
