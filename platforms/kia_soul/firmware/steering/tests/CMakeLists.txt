project(kia-soul-steering-tests)

add_library(
    kia-soul-steering
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/steering_control.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/dac/oscc_dac.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/pid/oscc_pid.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing/oscc_signal_smoothing.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    kia-soul-steering
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time
    ${CMAKE_SOURCE_DIR}/common/libs/dac
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing
    ${CMAKE_SOURCE_DIR}/common/testing/mocks)

add_executable(
    kia-soul-steering-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-steering-test
    PRIVATE
    kia-soul-steering
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    kia-soul-steering-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include)

add_custom_target(
    run-steering-tests
    DEPENDS
    kia-soul-steering-test
    COMMAND
    kia-soul-steering-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
