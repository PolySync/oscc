cmake_minimum_required(VERSION 2.8)

project(api-tests)

set(CMAKE_CFLAGS "-std=gnu11 -Os")
set(CMAKE_CXX_FLAGS "-std=gnu++11 -Os")

if(KIA_SOUL)
    add_definitions(-DKIA_SOUL)
elseif(KIA_SOUL_EV)
    add_definitions(-DKIA_SOUL_EV)
else()
    message(FATAL_ERROR "No platform selected - nothing will be tested")
endif()

option(KIA_SOUL "Build firmware for the petrol Kia Soul" OFF)
option(KIA_SOUL_EV "Build firmware for the Kia Soul EV" OFF)

add_library(
    api
    SHARED
    ${CMAKE_SOURCE_DIR}/../src/oscc.c
)

target_include_directories(
    api
    PRIVATE
    ${CMAKE_SOURCE_DIR}/../include)

add_executable(
    api-unit-test
    features/step_definitions/test.cpp)

target_link_libraries(
    api-unit-test
    PRIVATE
    api
    ${CMAKE_SOURCE_DIR}/../../firmware/common/testing/framework/cucumber-cpp/lib/libcucumber-cpp.a
    ${CMAKE_SOURCE_DIR}/../../firmware/common/testing/framework/cgreen/lib/libcgreen.so)

target_include_directories(
    api-unit-test
    PRIVATE
    ${CMAKE_SOURCE_DIR}/../include
    ${CMAKE_SOURCE_DIR}/../../firmware/common/testing/framework/cucumber-cpp/include
    ${CMAKE_SOURCE_DIR}/../../firmware/common/testing/framework/cgreen/include)

add_custom_target(
    run-api-unit-tests
    DEPENDS
    api-unit-test
    COMMAND
    api-unit-test --port=3902 >/dev/null & cucumber _2.0.0_ ${CMAKE_SOURCE_DIR}/features )

add_custom_target(
    run-api-property-tests
    COMMAND
    cargo test -- --test-threads=1 ;
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/property)