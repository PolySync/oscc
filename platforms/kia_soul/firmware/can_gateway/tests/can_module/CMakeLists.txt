project(kia-soul-can-gateway-tests-can-module)

generate_arduino_firmware(
    kia-soul-can-gateway-tests-can-module
    SKETCH src/can_module_test.ino
    SRCS ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can/mcp_can.cpp)

target_include_directories(
    kia-soul-can-gateway-tests-can-module
    PRIVATE
    include
    ${CMAKE_SOURCE_DIR}/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/arduino_libraries/mcp_can)
