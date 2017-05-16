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

    add_custom_target(
        run-tests
        DEPENDS
        run-brake-tests
        run-can-gateway-tests
        run-steering-tests
        run-throttle-tests)
else()
    set(CMAKE_TOOLCHAIN_FILE common/toolchain/ArduinoToolchain.cmake)

    cmake_minimum_required(VERSION 2.8)

    project(firmware)

    set(ARDUINO_DEFAULT_PORT /dev/ttyACM0)

    option(DEBUG "Enable debug mode" OFF)
    option(BUILD_KIA_SOUL "Build firmware for Kia Soul" OFF)

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    if(BUILD_KIA_SOUL)
        add_subdirectory(kia_soul/firmware)
    else()
        message(WARNING "No platform selected - no firmware will be built")
    endif()
endif()
