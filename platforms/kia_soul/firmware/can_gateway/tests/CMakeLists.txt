project(kia-soul-can-gateway-tests)

add_library(
    kia-soul-can-gateway
    SHARED
    ../src/communications.cpp
    ../src/globals.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/time/oscc_time.cpp
    ${CMAKE_SOURCE_DIR}/common/libs/can/oscc_can.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/Arduino_mock.cpp
    ${CMAKE_SOURCE_DIR}/common/testing/mocks/mcp_can_mock.cpp)

target_include_directories(
    kia-soul-can-gateway
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/libs/time
    ${CMAKE_SOURCE_DIR}/common/testing/mocks)

add_executable(
    kia-soul-can-gateway-test
    features/step_definitions/test.cpp)

target_link_libraries(
    kia-soul-can-gateway-test
    PRIVATE
    kia-soul-can-gateway
    /opt/cucumber-cpp/lib/libcucumber-cpp.a
    /opt/cgreen/lib/libcgreen.so
    ${Boost_THREAD_LIBRARY}
    ${Boost_SYSTEM_LIBRARY}
    ${Boost_REGEX_LIBRARY}
    ${Boost_DATE_TIME_LIBRARY}
    ${Boost_PROGRAM_OPTIONS_LIBRARY}
    ${Boost_FILESYSTEM_LIBRARY})

target_include_directories(
    kia-soul-can-gateway-test
    PRIVATE
    ../include
    ${CMAKE_SOURCE_DIR}/common/include
    ${CMAKE_SOURCE_DIR}/common/libs/can
    ${CMAKE_SOURCE_DIR}/common/testing/mocks
    /opt/cucumber-cpp/include
    /opt/cgreen/include)

add_custom_target(
    run-can-gateway-tests
    DEPENDS
    kia-soul-can-gateway-test
    COMMAND
    kia-soul-can-gateway-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_CURRENT_SOURCE_DIR}/features )
