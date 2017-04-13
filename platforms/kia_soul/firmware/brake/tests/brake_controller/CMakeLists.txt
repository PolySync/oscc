project(kia-soul-brake-tests-controller)

generate_arduino_firmware(
    kia-soul-brake-tests-controller
    SKETCH src/brake_controller_r0_test.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/arduino_libraries/FiniteStateMachine/FiniteStateMachine.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/PID/PID.c)

target_include_directories(
    kia-soul-brake-tests-controller
    PRIVATE
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/control_protocol_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/FiniteStateMachine
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/PID)
