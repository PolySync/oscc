project(brake-unit-tests)

set(CUCUMBER_PORT_BRAKE "39${PORT_SUFFIX}")

add_library(
    brake
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/brake_control.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/fault_check/oscc_check.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/dac/oscc_dac.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    brake
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
    brake-unit-test
    features/step_definitions/test.cpp)

target_link_libraries(
    brake-unit-test
    PRIVATE
    brake
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    brake-unit-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/fault_check
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include
    ${CMAKE_SOURCE_DIR}/../api/include)

file(WRITE ${CMAKE_CURRENT_LIST_DIR}/features/step_definitions/cucumber.wire "host: localhost\nport: ${CUCUMBER_PORT_BRAKE}")

add_custom_target(
    run-brake-unit-tests
    DEPENDS
    brake-unit-test
    COMMAND
    brake-unit-test --port=${CUCUMBER_PORT_BRAKE} >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )

add_custom_target(
    run-brake-property-tests
    COMMAND
    cargo +1.20.0 test -- --test-threads=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)
