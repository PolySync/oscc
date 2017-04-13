project(kia-soul-can-steering-tests-controller)

generate_arduino_firmware(
    kia-soul-can-steering-tests-controller
    SKETCH src/steering_controller_r0_test.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx/DAC_MCP49xx.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp)

target_include_directories(
    kia-soul-can-steering-tests-controller
    PRIVATE
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can)
