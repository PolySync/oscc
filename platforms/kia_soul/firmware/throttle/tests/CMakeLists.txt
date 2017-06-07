project(kia-soul-throttle-tests)

add_library(
    kia-soul-throttle
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/throttle_control.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/dac/oscc_dac.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing/oscc_signal_smoothing.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    kia-soul-throttle
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time
    ${CMAKE_SOURCE_DIR}/common/libs/dac
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing
    ${CMAKE_SOURCE_DIR}/common/testing/mocks)

add_executable(
    kia-soul-throttle-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-throttle-test
    PRIVATE
    kia-soul-throttle
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    kia-soul-throttle-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include)

add_custom_target(
    run-throttle-tests
    DEPENDS
    kia-soul-throttle-test
    COMMAND
    kia-soul-throttle-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
