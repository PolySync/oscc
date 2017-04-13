project(kia-soul-steering)

set(ARDUINO_DEFAULT_BOARD uno)

generate_arduino_firmware(
    kia-soul-steering
    SKETCH src/steering_control_module.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx/DAC_MCP49xx.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/PID/PID.c)

target_include_directories(
    kia-soul-steering
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/control_protocol_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/PID)

add_subdirectory(tests)
