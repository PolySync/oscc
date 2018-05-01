project(can-gateway-unit-tests)

set(CUCUMBER_PORT_GATEWAY "39${PORT_SUFFIX}")

add_library(
    can-gateway
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp)

target_include_directories(
    can-gateway
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/../api/include)

add_executable(
    can-gateway-unit-test
    features/step_definitions/test.cpp)

target_link_libraries(
    can-gateway-unit-test
    PRIVATE
    can-gateway
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    can-gateway-unit-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include
    ${CMAKE_SOURCE_DIR}/../api/include)

file(WRITE ${CMAKE_CURRENT_LIST_DIR}/features/step_definitions/cucumber.wire "host: localhost\nport: ${CUCUMBER_PORT_GATEWAY}")

add_custom_target(
    run-can-gateway-unit-tests
    DEPENDS
    can-gateway-unit-test
    COMMAND
    can-gateway-unit-test --port=${CUCUMBER_PORT_GATEWAY} >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
