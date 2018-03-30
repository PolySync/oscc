cmake_minimum_required(VERSION 2.8)

set(OSCC_FIRMWARE_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

#Required by Arduino?
#set(CMAKE_CFLAGS "-std=gnu11 -Os")
#set(CMAKE_CXX_FLAGS "-std=gnu++11 -Os")

#can use set instead of option like below?
option(DEBUG "Enable debug mode" OFF)
option(KIA_SOUL "Build firmware for the petrol Kia Soul" OFF)
option(KIA_SOUL_EV "Build firmware for the Kia Soul EV" OFF)
option(KIA_NIRO "Build firmware for the Kia Niro" OFF)
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

#target_compile_options (or something) for setting locally rather than globally
if(DEBUG)
    add_definitions(-DDEBUG)
endif()

if(KIA_SOUL)
    add_definitions(-DKIA_SOUL)
elseif(KIA_SOUL_EV)
    add_definitions(-DKIA_SOUL_EV)
elseif(KIA_NIRO)
    add_definitions(-DKIA_NIRO)
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
