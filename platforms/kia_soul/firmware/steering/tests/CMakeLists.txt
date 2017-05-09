project(kia-soul-steering-tests)

add_library(
    kia-soul-steering
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/steering_control.cpp
    ../../../../common/libs/time/oscc_time.cpp
    ../../../../common/libs/can/oscc_can.cpp
    ../../../../common/libs/dac/oscc_dac.cpp
    ../../../../common/libs/pid/oscc_pid.cpp
    ../../../../common/testing/mocks/
    ../../../../common/testing/mocks/Arduino_mock.cpp
    ../../../../common/testing/mocks/mcp_can_mock.cpp
    ../../../../common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    kia-soul-steering
    PRIVATE
    ../include
    ../../../../common/include
    ../../../../common/libs/can
    ../../../../common/libs/time
    ../../../../common/libs/dac
    ../../../../common/libs/pid
    ../../../../common/testing/mocks)

add_executable(
    kia-soul-steering-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-steering-test
    PRIVATE
    kia-soul-steering
    /opt/cucumber-cpp/lib/libcucumber-cpp.a
    /opt/cgreen/lib/libcgreen.so
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY})

target_include_directories(
    kia-soul-steering-test
    PRIVATE
    ../include
    ../../../../common/include
    ../../../../common/libs/can
    ../../../../common/libs/pid
    ../../../../common/testing/mocks
    /opt/cucumber-cpp/include
    /opt/cgreen/include)

add_custom_target(
    run-steering-tests
    DEPENDS
    kia-soul-steering-test
    COMMAND
    kia-soul-steering-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
