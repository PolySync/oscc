project(kia-soul-brake-tests-controller)

generate_arduino_firmware(
    kia-soul-brake-tests-controller
    SKETCH src/brake_controller_r0_test.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/common/libs/FiniteStateMachine/FiniteStateMachine.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/PID/PID.c
    ${CMAKE_SOURCE_DIR}/common/libs/serial/serial.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/time.cpp)

target_include_directories(
    kia-soul-brake-tests-controller
    PRIVATE
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/FiniteStateMachine
    ${CMAKE_SOURCE_DIR}/common/libs/mcp_can
    ${CMAKE_SOURCE_DIR}/common/libs/PID
    ${CMAKE_SOURCE_DIR}/common/libs/serial
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time)
