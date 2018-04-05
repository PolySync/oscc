cmake_minimum_required(VERSION 2.8)

include(${CMAKE_CURRENT_LIST_DIR}/OsccConfig.cmake)
set(OSCC_FIRMWARE_ROOT ${CMAKE_CURRENT_LIST_DIR}/..)

set(DEBUG OFF CACHE BOOL "Enable debug mode")
set(BRAKE_STARTUP_TEST ON CACHE BOOL "Enable brake startup sensor tests")
set(STEERING_OVERRIDE ON CACHE BOOL "Enable steering override")

set(SERIAL_PORT_BRAKE "/dev/ttyACM0" CACHE STRING "Serial port of the brake module")
set(SERIAL_BAUD_BRAKE "115200" CACHE STRING "Serial baud rate of the brake module")

set(SERIAL_PORT_CAN_GATEWAY "/dev/ttyACM0" CACHE STRING "Serial port of the CAN gateway module")
set(SERIAL_BAUD_CAN_GATEWAY "115200" CACHE STRING "Serial baud rate of the CAN gateway module")

set(SERIAL_PORT_STEERING "/dev/ttyACM0" CACHE STRING "Serial port of the steering module")
set(SERIAL_BAUD_STEERING "115200" CACHE STRING "Serial baud rate of the steering module")

set(SERIAL_PORT_THROTTLE "/dev/ttyACM0" CACHE STRING "Serial port of the throttle module")
set(SERIAL_BAUD_THROTTLE "115200" CACHE STRING "Serial baud rate of the throttle module")

set(SERIAL_PORT_NULL "/dev/ttyACM0" CACHE STRING "Serial port of the NULL module")
set(SERIAL_BAUD_NULL "115200" CACHE STRING "Serial baud rate of the NULL module")

if(DEBUG)
    add_definitions(-DDEBUG)
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
