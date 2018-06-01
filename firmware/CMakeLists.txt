cmake_minimum_required(VERSION 2.8)

set(ARDUINO_DEFAULT_BOARD uno)

if(TESTS)
    include(cmake/OsccConfig.cmake)

    project(tests)

    set(PORT_SUFFIX "02" CACHE STRING "Local host server port to use for cucumber tests")

    if(TESTS)
        add_definitions(-DTESTS)
    endif()

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    if(VEHICLE STREQUAL "kia_soul")
        add_subdirectory(brake/kia_soul_petrol/tests)
    else()
        add_subdirectory(brake/kia_soul_ev_niro/tests)
    endif()

    add_subdirectory(can_gateway/tests)
    add_subdirectory(steering/tests)
    add_subdirectory(throttle/tests)
    add_subdirectory(common/libs/pid/tests)

    add_custom_target(
        run-unit-tests
        DEPENDS
        run-brake-unit-tests
        run-can-gateway-unit-tests
        run-steering-unit-tests
        run-throttle-unit-tests)

    add_custom_target(
        run-property-tests
        DEPENDS
        run-brake-property-tests
        run-steering-property-tests
        run-throttle-property-tests)

    add_custom_target(
        run-library-property-tests
        DEPENDS
        run-pid-library-property-tests)

    add_custom_target(
        run-all-tests
        DEPENDS
        run-unit-tests
        run-property-tests)
else()
    set(CMAKE_TOOLCHAIN_FILE common/toolchain/ArduinoToolchain.cmake)

    include(cmake/OsccFirmware.cmake)

    project(firmware)

    add_subdirectory(brake)
    add_subdirectory(can_gateway)
    add_subdirectory(null)
    add_subdirectory(steering)
    add_subdirectory(throttle)

    add_custom_target(
        all-upload
        DEPENDS
        brake-upload
        can-gateway-upload
        steering-upload
        throttle-upload)
endif()
