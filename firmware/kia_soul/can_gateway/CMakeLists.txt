project(kia-soul-can-gateway)

set(ARDUINO_DEFAULT_BOARD uno)

generate_arduino_firmware(
    kia-soul-can-gateway
    SKETCH src/can_gateway.ino
    SRCS ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp)

target_include_directories(
    kia-soul-can-gateway
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/control_protocol_can
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can)

add_subdirectory(tests)
