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
    ${CMAKE_SOURCE_DIR}/common/libs/signal_smoothing/signal_smoothing.cpp
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
    ${CMAKE_SOURCE_DIR}/common/testing/mocks)

add_executable(
    kia-soul-brake-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-brake-test
    PRIVATE
    kia-soul-brake
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    kia-soul-brake-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/pid
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/common/testing/framework/cgreen/include)

add_custom_target(
    run-brake-tests
    DEPENDS
    kia-soul-brake-test
    COMMAND
    kia-soul-brake-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
