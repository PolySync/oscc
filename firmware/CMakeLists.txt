if(TESTS)
    cmake_minimum_required(VERSION 2.8)

    project(tests)

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    add_subdirectory(kia_soul/brake/tests)
    add_subdirectory(kia_soul/can_gateway/tests)
    add_subdirectory(kia_soul/steering/tests)
    add_subdirectory(kia_soul/throttle/tests)
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

    set(SERIAL_PORT_BRAKE "/dev/ttyACM0" CACHE STRING "Serial port of the brake module")
    set(SERIAL_BAUD_BRAKE "115200" CACHE STRING "Serial baud rate of the brake module")

    set(SERIAL_PORT_CAN_GATEWAY "/dev/ttyACM0" CACHE STRING "Serial port of the CAN gateway module")
    set(SERIAL_BAUD_CAN_GATEWAY "115200" CACHE STRING "Serial baud rate of the CAN gateway module")

    set(SERIAL_PORT_STEERING "/dev/ttyACM0" CACHE STRING "Serial port of the steering module")
    set(SERIAL_BAUD_STEERING "115200" CACHE STRING "Serial baud rate of the steering module")

    set(SERIAL_PORT_THROTTLE "/dev/ttyACM0" CACHE STRING "Serial port of the throttle module")
    set(SERIAL_BAUD_THROTTLE "115200" CACHE STRING "Serial baud rate of the throttle module")

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    if(BUILD_KIA_SOUL)
        add_subdirectory(kia_soul)
    else()
        message(WARNING "No platform selected - no firmware will be built")
    endif()
endif()
