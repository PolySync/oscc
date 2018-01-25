if(TESTS)
    cmake_minimum_required(VERSION 2.8)

    project(tests)

    if(TESTS)
        add_definitions(-DTESTS)
    endif()

    if(DEBUG)
        add_definitions(-DDEBUG)
    endif()

    if(KIA_SOUL)
        add_definitions(-DKIA_SOUL)
        add_subdirectory(brake/kia_soul_petrol/tests)
    elseif(KIA_SOUL_EV)
        add_definitions(-DKIA_SOUL_EV)
        add_subdirectory(brake/kia_soul_ev/tests)
    else()
        message(FATAL_ERROR "No platform selected - no firmware will be tested")
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

    cmake_minimum_required(VERSION 2.8)

    project(firmware)

    set(CMAKE_CFLAGS "-std=gnu11 -Os")
    set(CMAKE_CXX_FLAGS "-std=gnu++11 -Os")

    option(DEBUG "Enable debug mode" OFF)
    option(KIA_SOUL "Build firmware for the petrol Kia Soul" OFF)
    option(KIA_SOUL_EV "Build firmware for the Kia Soul EV" OFF)
    option(BRAKE_STARTUP_TEST "Enable brake startup sensor tests" ON)
    option(STEERING_OVERRIDE "Enable steering override" ON)

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

    if(KIA_SOUL)
        add_definitions(-DKIA_SOUL)
    elseif(KIA_SOUL_EV)
        add_definitions(-DKIA_SOUL_EV)
    else()
        message(FATAL_ERROR "No platform selected - no firmware will be built")
    endif()

    if(KIA_SOUL AND BRAKE_STARTUP_TEST)
        add_definitions(-DBRAKE_STARTUP_TEST)
    elseif(KIA_SOUL AND NOT BRAKE_STARTUP_TEST)
        message(WARNING "Brake sensor startup tests disabled")
    endif()

    if(STEERING_OVERRIDE)
        add_definitions(-DSTEERING_OVERRIDE)
        message(WARNING "Steering override is enabled. This is an experimental feature! Attempting to grab the steering wheel while the system is active could result in serious injury. The preferred method of operator override for steering is to utilize the brake pedal or E-stop button.")
    endif()

    add_subdirectory(brake)
    add_subdirectory(can_gateway)
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
