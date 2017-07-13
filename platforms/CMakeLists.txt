if(TESTS)
    cmake_minimum_required(VERSION 2.8)

    project(tests)

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    add_subdirectory(kia_soul/firmware/brake/tests)
    add_subdirectory(kia_soul/firmware/can_gateway/tests)
    add_subdirectory(kia_soul/firmware/steering/tests)
    add_subdirectory(kia_soul/firmware/throttle/tests)
    add_subdirectory(common/libs/pid/tests)

    add_custom_target(
        run-kia-soul-unit-tests
        DEPENDS
        run-kia-soul-brake-unit-tests
        run-kia-soul-can-gateway-unit-tests
        run-kia-soul-steering-unit-tests
        run-kia-soul-throttle-unit-tests)

    add_custom_target(
        run-kia-soul-property-tests
        DEPENDS
        run-kia-soul-brake-property-tests
        run-kia-soul-steering-property-tests
        run-kia-soul-throttle-property-tests)

    add_custom_target(
        run-library-property-tests
        DEPENDS
        run-pid-library-property-tests)

    add_custom_target(
        run-unit-tests
        DEPENDS
        run-kia-soul-unit-tests)

    add_custom_target(
        run-property-tests
        DEPENDS
        run-kia-soul-property-tests
        run-library-property-tests)

    add_custom_target(
        run-all-tests
        DEPENDS
        run-unit-tests
        run-property-tests)
else()
    set(CMAKE_TOOLCHAIN_FILE common/toolchain/ArduinoToolchain.cmake)

    cmake_minimum_required(VERSION 2.8)

    project(firmware)

    option(DEBUG "Enable debug mode" OFF)
    option(BUILD_KIA_SOUL "Build firmware for Kia Soul" OFF)

    set(ARDUINO_DEFAULT_PORT /dev/ttyACM0)
    set(ARDUINO_DEFAULT_BAUDRATE 115200)

    add_definitions(-DSERIAL_BAUD=${ARDUINO_DEFAULT_BAUDRATE})

    add_custom_target(
        monitor
        COMMAND screen ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

    add_custom_target(
        monitor-log
        COMMAND screen -L ${ARDUINO_DEFAULT_PORT} ${ARDUINO_DEFAULT_BAUDRATE})

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    if(BUILD_KIA_SOUL)
        add_subdirectory(kia_soul/firmware)
    else()
        message(WARNING "No platform selected - no firmware will be built")
    endif()
endif()
