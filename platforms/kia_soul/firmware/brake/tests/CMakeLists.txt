project(kia-soul-brake-tests)

add_library(
    kia-soul-brake
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/brake_control.cpp
    ../src/master_cylinder.cpp
    ../src/helper.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/pid/oscc_pid.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing/oscc_signal_smoothing.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    kia-soul-brake
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/kia_soul/firmware)

add_executable(
    kia-soul-brake-unit-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-brake-unit-test
    PRIVATE
    kia-soul-brake
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    kia-soul-brake-unit-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include
    ${CMAKE_SOURCE_DIR}/kia_soul/firmware)

add_custom_target(
    run-kia-soul-brake-unit-tests
    DEPENDS
    kia-soul-brake-unit-test
    COMMAND
    kia-soul-brake-unit-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )

add_custom_target(
    run-kia-soul-brake-property-tests
    COMMAND
    cargo test -- --test-threads=1
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/property)
