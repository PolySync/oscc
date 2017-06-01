#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
#include <sys/timeb.h>
#include <unistd.h>

#include "Arduino.h"
#include "communications.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "steering_control.h"
#include "steering_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "globals.h"

using namespace cgreen;


extern unsigned long g_mock_arduino_millis_return;
extern uint8_t g_mock_arduino_digital_write_pin;
extern uint8_t g_mock_arduino_digital_write_val;
extern int g_mock_arduino_analog_read_return;

extern INT8U g_mock_mcp_can_check_receive_return;

extern INT32U g_mock_mcp_can_read_msg_buf_id;
extern INT8U g_mock_mcp_can_read_msg_buf_buf[8];

extern INT32U g_mock_mcp_can_send_msg_buf_id;
extern INT8U g_mock_mcp_can_send_msg_buf_ext;
extern INT8U g_mock_mcp_can_send_msg_buf_len;
extern INT8U *g_mock_mcp_can_send_msg_buf_buf;

extern unsigned short g_mock_dac_output_a;
extern unsigned short g_mock_dac_output_b;

extern kia_soul_steering_control_state_s g_steering_control_state;


// return to known state before every scenario
BEFORE()
{
    g_mock_mcp_can_check_receive_return = -1;
    g_mock_mcp_can_read_msg_buf_id = 0;
    g_mock_arduino_millis_return = 555;

    memset(
        &g_mock_mcp_can_read_msg_buf_buf,
        0,
        sizeof(g_mock_mcp_can_read_msg_buf_buf));

    // Do not clear previous_steering_wheel_angle because it is needed for
    // scenario about receiving a command
    g_steering_control_state.enabled = 0;
    g_steering_control_state.operator_override = 0;
    g_steering_control_state.current_steering_wheel_angle = 0;
    g_steering_control_state.commanded_steering_wheel_angle = 0;

    g_mock_arduino_digital_write_pin = UINT8_MAX;
    g_mock_arduino_digital_write_val = UINT8_MAX;
    g_mock_arduino_analog_read_return = INT_MAX;
    g_mock_dac_output_a = USHRT_MAX;
    g_mock_dac_output_b = USHRT_MAX;
}


GIVEN("^steering control is enabled$")
{
    g_steering_control_state.enabled = 1;
}


GIVEN("^steering control is disabled$")
{
    g_steering_control_state.enabled = 0;
}


GIVEN("^the torque sensors have a reading of (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return = sensor_val;
}


GIVEN("^the previous steering wheel angle command was (.*)$")
{
    REGEX_PARAM(int, command);

    g_steering_control_state.commanded_steering_wheel_angle = command;
}


THEN("^control should be enabled$")
{
    assert_that(
        g_steering_control_state.enabled,
        is_equal_to(1));

    assert_that(
        g_mock_arduino_digital_write_pin,
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val,
        is_equal_to(HIGH));
}


THEN("^control should be disabled$")
{
    assert_that(
        g_steering_control_state.enabled,
        is_equal_to(0));

    assert_that(
        g_mock_arduino_digital_write_pin,
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val,
        is_equal_to(LOW));
}


THEN("^(.*) should be written to DAC A$")
{
    REGEX_PARAM(int, dac_output_a);

    assert_that(
        g_mock_dac_output_a,
        is_equal_to(dac_output_a));
}


THEN("^(.*) should be written to DAC B$")
{
    REGEX_PARAM(int, dac_output_b);

    assert_that(
        g_mock_dac_output_b,
        is_equal_to(dac_output_b));
}
