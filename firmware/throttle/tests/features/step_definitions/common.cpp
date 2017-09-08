#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
#include <sys/timeb.h>
#include <unistd.h>

#include "Arduino.h"
#include "communications.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "throttle_control.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "globals.h"

using namespace cgreen;


extern uint8_t g_mock_arduino_digital_write_pins[100];
extern uint8_t g_mock_arduino_digital_write_val[100];
extern int g_mock_arduino_digital_write_count;

extern int g_mock_arduino_analog_read_return[100];

extern uint8_t g_mock_mcp_can_check_receive_return;
extern uint32_t g_mock_mcp_can_read_msg_buf_id;
extern uint8_t g_mock_mcp_can_read_msg_buf_buf[8];
extern uint32_t g_mock_mcp_can_send_msg_buf_id;
extern uint8_t g_mock_mcp_can_send_msg_buf_ext;
extern uint8_t g_mock_mcp_can_send_msg_buf_len;
extern uint8_t *g_mock_mcp_can_send_msg_buf_buf;

extern unsigned short g_mock_dac_output_a;
extern unsigned short g_mock_dac_output_b;

extern volatile throttle_control_state_s g_throttle_control_state;


// return to known state before every scenario
BEFORE()
{
    g_mock_arduino_digital_write_count = 0;
    memset(&g_mock_arduino_digital_write_pins, 0, sizeof(g_mock_arduino_digital_write_pins));
    memset(&g_mock_arduino_digital_write_val, 0, sizeof(g_mock_arduino_digital_write_val));

    g_mock_arduino_analog_read_return[0] = INT_MAX;
    g_mock_arduino_analog_read_return[1] = INT_MAX;

    g_mock_mcp_can_check_receive_return = UINT8_MAX;
    g_mock_mcp_can_read_msg_buf_id = UINT32_MAX;
    memset(&g_mock_mcp_can_read_msg_buf_buf, 0, sizeof(g_mock_mcp_can_read_msg_buf_buf));

    g_mock_mcp_can_send_msg_buf_id = UINT32_MAX;
    g_mock_mcp_can_send_msg_buf_ext = UINT8_MAX;
    g_mock_mcp_can_send_msg_buf_len = UINT8_MAX;

    g_mock_dac_output_a = USHRT_MAX;
    g_mock_dac_output_b = USHRT_MAX;

    g_throttle_control_state.enabled = false;
    g_throttle_control_state.operator_override = false;
    g_throttle_control_state.dtcs = 0;
}


GIVEN("^throttle control is enabled$")
{
    g_throttle_control_state.enabled = 1;
}


GIVEN("^throttle control is disabled$")
{
    g_throttle_control_state.enabled = 0;
}


GIVEN("^the accelerator position sensors have a reading of (.*)$")
{
    REGEX_PARAM(int, sensor_val);

    g_mock_arduino_analog_read_return[0] = sensor_val;
    g_mock_arduino_analog_read_return[1] = sensor_val;
}


GIVEN("^the operator has applied (.*) to the accelerator$")
{

    REGEX_PARAM(int, throttle_sensor_val);

    g_mock_arduino_analog_read_return[0] = throttle_sensor_val;
    g_mock_arduino_analog_read_return[1] = throttle_sensor_val;

    check_for_operator_override();
}


THEN("^control should be enabled$")
{
    assert_that(
        g_throttle_control_state.enabled,
        is_equal_to(1));

    assert_that(
        g_mock_arduino_digital_write_pins[0],
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val[0],
        is_equal_to(HIGH));
}


THEN("^control should be disabled$")
{
    assert_that(
        g_throttle_control_state.enabled,
        is_equal_to(0));

    assert_that(
        g_mock_arduino_digital_write_pins[0],
        is_equal_to(PIN_SPOOF_ENABLE));

    assert_that(
        g_mock_arduino_digital_write_val[0],
        is_equal_to(LOW));
}


THEN("^(.*) should be sent to DAC A$")
{
    REGEX_PARAM(int, dac_output_a);

    assert_that(
        g_mock_dac_output_a,
        is_equal_to(dac_output_a));
}


THEN("^(.*) should be sent to DAC B$")
{
    REGEX_PARAM(int, dac_output_b);

    assert_that(
        g_mock_dac_output_b,
        is_equal_to(dac_output_b));
}
