project(kia-soul-brake-tests-bench)

generate_arduino_firmware(
    kia-soul-brake-tests-bench
    SKETCH src/brake_bench_r0_test.ino
    SRCS ${CMAKE_SOURCE_DIR}/common/arduino_libraries/mcp_can/mcp_can.cpp)

target_include_directories(
    kia-soul-brake-tests-bench
    PRIVATE
    ${CMAKE_SOURCE_DIR}/common/arduino_libraries/can_frame
    ${CMAKE_SOURCE_DIR}/common/arduino_libraries/common
    ${CMAKE_SOURCE_DIR}/common/arduino_libraries/mcp_can)
