#pragma once

#include <map>
#include <string>
#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>

#include "Arduino.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "can_protocols/fault_can_protocol.h"
#include "vehicles.h"

using namespace cgreen;

extern uint8_t g_mock_arduino_digital_write_pins[100];
extern uint8_t g_mock_arduino_digital_write_val[100];
extern int g_mock_arduino_digital_write_count;

extern int g_mock_arduino_analog_read_return[100];
extern int g_mock_arduino_analog_write_count;
extern uint8_t g_mock_arduino_analog_write_pins[100];
extern int g_mock_arduino_analog_write_val[100];

extern uint8_t g_mock_mcp_can_check_receive_return;
extern uint32_t g_mock_mcp_can_read_msg_buf_id;
extern uint8_t g_mock_mcp_can_read_msg_buf_buf[8];
extern uint32_t g_mock_mcp_can_send_msg_buf_id;
extern uint8_t g_mock_mcp_can_send_msg_buf_ext;
extern uint8_t g_mock_mcp_can_send_msg_buf_len;
extern uint8_t *g_mock_mcp_can_send_msg_buf_buf;

extern unsigned short g_mock_dac_output_a;
extern unsigned short g_mock_dac_output_b;

extern volatile unsigned long g_mock_arduino_millis_return;
extern volatile unsigned long g_mock_arduino_micros_return;

std::map<std::string, int> vehicle_defines;
const unsigned long Hysteresis_Time = FAULT_HYSTERESIS;


// return to known state before every scenario
BEFORE()
{
    g_mock_arduino_digital_write_count = 0;
    memset(&g_mock_arduino_digital_write_pins, 0, sizeof(g_mock_arduino_digital_write_pins));
    memset(&g_mock_arduino_digital_write_val, 0, sizeof(g_mock_arduino_digital_write_val));

    g_mock_arduino_analog_read_return[0] = INT_MAX;
    g_mock_arduino_analog_read_return[1] = INT_MAX;
    g_mock_arduino_analog_write_count = 0;
    memset(&g_mock_arduino_analog_write_pins, 0, sizeof(g_mock_arduino_analog_write_pins));
    memset(&g_mock_arduino_analog_write_val, 0, sizeof(g_mock_arduino_analog_write_val));

    g_mock_mcp_can_check_receive_return = UINT8_MAX;
    g_mock_mcp_can_read_msg_buf_id = UINT32_MAX;
    memset(&g_mock_mcp_can_read_msg_buf_buf, 0, sizeof(g_mock_mcp_can_read_msg_buf_buf));

    g_mock_mcp_can_send_msg_buf_id = UINT32_MAX;
    g_mock_mcp_can_send_msg_buf_ext = UINT8_MAX;
    g_mock_mcp_can_send_msg_buf_len = UINT8_MAX;

    g_mock_dac_output_a = USHRT_MAX;
    g_mock_dac_output_b = USHRT_MAX;

    g_vcm_control_state.enabled = false;
    g_vcm_control_state.operator_override = false;
    g_vcm_control_state.dtcs = 0;

    // A small amount of time elapsed after boot
    g_mock_arduino_millis_return = 1;

    //Make sure dictionary for looking up vehicle specific defines is correct
    vehicle_defines["STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN"] =
        STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN;
    vehicle_defines["STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX"] =
        STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX;
    vehicle_defines["STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN"] =
        STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN;
    vehicle_defines["STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX"] =
        STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX;
    vehicle_defines["THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN"] =
        THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN;
    vehicle_defines["THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX"] =
        THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX;
    vehicle_defines["THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN"] =
        THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN;
    vehicle_defines["THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX"] =
        THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX;
#ifndef KIA_SOUL
    vehicle_defines["BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN"] =
        BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN;
    vehicle_defines["BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX"] =
        BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX;
    vehicle_defines["BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN"] =
        BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN;
    vehicle_defines["BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX"] =
        BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX;
#endif
}

GIVEN("^(\\w+) control is enabled$")
{
    REGEX_PARAM(std::string, module_name);

    assert_that(module_name, is_equal_to_string(FIRMWARE_NAME));

    g_vcm_control_state.enabled = 1;
}

GIVEN("^(\\w+) control is disabled$")
{
    REGEX_PARAM(std::string, module_name);

    assert_that(module_name, is_equal_to_string(FIRMWARE_NAME));

    g_vcm_control_state.enabled = 0;
}
