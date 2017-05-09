project(kia-soul-throttle-tests)

add_library(
    kia-soul-throttle
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ../src/throttle_control.cpp
    ../../../../common/libs/time/oscc_time.cpp
    ../../../../common/libs/can/oscc_can.cpp
    ../../../../common/libs/dac/oscc_dac.cpp
    ../../../../common/testing/mocks/
    ../../../../common/testing/mocks/Arduino_mock.cpp
    ../../../../common/testing/mocks/mcp_can_mock.cpp
    ../../../../common/testing/mocks/DAC_MCP49xx_mock.cpp)

target_include_directories(
    kia-soul-throttle
    PRIVATE
    ../include
    ../../../../common/include
    ../../../../common/libs/can
    ../../../../common/libs/time
    ../../../../common/libs/dac
    ../../../../common/testing/mocks)

add_executable(
    kia-soul-throttle-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-throttle-test
    PRIVATE
    kia-soul-throttle
    /opt/cucumber-cpp/lib/libcucumber-cpp.a
    /opt/cgreen/lib/libcgreen.so
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY})

target_include_directories(
    kia-soul-throttle-test
    PRIVATE
    ../include
    ../../../../common/include
    ../../../../common/libs/can
    ../../../../common/testing/mocks
    /opt/cucumber-cpp/include
    /opt/cgreen/include)

add_custom_target(
    run-throttle-tests
    DEPENDS
    kia-soul-throttle-test
    COMMAND
    kia-soul-throttle-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
