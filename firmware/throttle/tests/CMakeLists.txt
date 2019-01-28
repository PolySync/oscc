project(throttle-unit-tests)

set(CUCUMBER_PORT_THROTTLE "39${PORT_SUFFIX}")

add_library(
    throttle
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/throttle_control.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/fault_check/oscc_check.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/dac/oscc_dac.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    throttle
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/fault_check
    ${CMAKE_SOURCE_DIR}/common/libs/dac
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/../api/include)

add_executable(
    throttle-unit-test
    features/step_definitions/test.cpp)

target_link_libraries(
    throttle-unit-test
    PRIVATE
    throttle
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    throttle-unit-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/fault_check
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include
    ${CMAKE_SOURCE_DIR}/../api/include)

file(WRITE ${CMAKE_CURRENT_LIST_DIR}/features/step_definitions/cucumber.wire "host: localhost\nport: ${CUCUMBER_PORT_THROTTLE}")


add_custom_target(
    run-throttle-unit-tests
    DEPENDS
    throttle-unit-test
    COMMAND
    throttle-unit-test --port=${CUCUMBER_PORT_THROTTLE} >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )

add_custom_target(
    run-throttle-property-tests
    COMMAND
    cargo +1.20.0 test -- --test-threads=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)
