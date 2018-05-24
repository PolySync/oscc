project(brake-utils-serial-actuator)

generate_arduino_firmware(
    brake-utils-serial-actuator
    SRCS
    ${OSCC_FIRMWARE_ROOT}/common/libs/arduino_init/arduino_init.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/mcp_can/mcp_can.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/pid/oscc_pid.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/serial/oscc_serial.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/can/oscc_can.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/timer/oscc_timer.cpp
    ${OSCC_FIRMWARE_ROOT}/common/libs/fault_check/oscc_check.cpp
    ../../src/globals.cpp
    ../../src/accumulator.cpp
    ../../src/helper.cpp
    ../../src/master_cylinder.cpp
    ../../src/brake_control.cpp
    ../../src/communications.cpp
    ../../src/init.cpp
    src/serial_actuator.cpp
    BOARD_CPU atmega2560
    BOARD mega)

target_include_directories(
    brake-utils-serial-actuator
    PRIVATE
    ../../include
    ${OSCC_FIRMWARE_ROOT}/common/include
    ${OSCC_FIRMWARE_ROOT}/common/libs/arduino_init
    ${OSCC_FIRMWARE_ROOT}/common/libs/mcp_can
    ${OSCC_FIRMWARE_ROOT}/common/libs/pid
    ${OSCC_FIRMWARE_ROOT}/common/libs/serial
    ${OSCC_FIRMWARE_ROOT}/common/libs/can
    ${OSCC_FIRMWARE_ROOT}/common/libs/timer
    ${OSCC_FIRMWARE_ROOT}/common/libs/fault_check
    ${OSCC_FIRMWARE_ROOT}/../api/include)
