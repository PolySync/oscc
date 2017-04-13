project(kia-soul-throttle)

set(ARDUINO_DEFAULT_BOARD uno)

generate_arduino_firmware(
    kia-soul-throttle
    SKETCH src/throttle_control_module.ino
    SRCS
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx/DAC_MCP49xx.cpp
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp)

target_include_directories(
    kia-soul-throttle
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/control_protocol_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/DAC_MCP49xx
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can)
