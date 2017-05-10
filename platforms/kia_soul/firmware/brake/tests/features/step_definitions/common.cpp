#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
#include <sys/timeb.h>
#include <unistd.h>

#include "Arduino.h"
#include "communications.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "brake_control.h"
#include "brake_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "globals.h"

using namespace cgreen;


extern unsigned long g_mock_arduino_millis_return;
extern uint8_t g_mock_arduino_analog_write_pins[100];
extern int g_mock_arduino_analog_write_val[100];
extern int g_mock_arduino_analog_write_count;
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

extern kia_soul_brake_control_state_s g_brake_control_state;


// return to known state before every scenario
BEFORE()
{
    g_mock_mcp_can_check_receive_return = -1;
    g_mock_mcp_can_read_msg_buf_id = 0;
    g_mock_arduino_millis_return = 555;

    memset(
        &g_mock_arduino_analog_write_pins,
        0,
        sizeof(g_mock_arduino_analog_write_pins));

    memset(
        &g_mock_arduino_analog_write_val,
        0,
        sizeof(g_mock_arduino_analog_write_val));

    g_mock_arduino_analog_write_count = 0;

    memset(
        &g_mock_mcp_can_read_msg_buf_buf,
        0,
        sizeof(g_mock_mcp_can_read_msg_buf_buf));

    memset(
        &g_brake_control_state,
        0,
        sizeof(g_brake_control_state));

    g_mock_arduino_digital_write_pin = UINT8_MAX;
    g_mock_arduino_digital_write_val = UINT8_MAX;
    g_mock_arduino_analog_read_return = INT_MAX;
}


GIVEN("^brake control is enabled$")
{
    g_brake_control_state.enabled = 1;
}


GIVEN("^brake control is disabled$")
{
    g_brake_control_state.enabled = 0;
}


GIVEN("^the previous brake pedal position command was (.*)$")
{
    REGEX_PARAM(int, command);

    g_brake_control_state.commanded_pedal_position = command;
}



THEN("^control should be enabled$")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(1));

    assert_that(
        g_mock_arduino_analog_write_pins[0],
        is_equal_to(PIN_MASTER_CYLINDER_SOLENOID));

    assert_that(
        g_mock_arduino_analog_write_val[0],
        is_equal_to(SOLENOID_PWM_ON));

    assert_that(
        g_mock_arduino_analog_write_pins[1],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[1],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[2],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[2],
        is_equal_to(SOLENOID_PWM_OFF));
}


THEN("^control should be disabled$")
{
    assert_that(
        g_brake_control_state.enabled,
        is_equal_to(0));

    // turn actuator solenoids off
    assert_that(
        g_mock_arduino_analog_write_pins[0],
        is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[0],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[1],
        is_equal_to(PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[1],
        is_equal_to(SOLENOID_PWM_OFF));

    // turn release solenoids on
    assert_that(
        g_mock_arduino_analog_write_pins[2],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[2],
        is_equal_to(SOLENOID_PWM_ON));

    assert_that(
        g_mock_arduino_analog_write_pins[3],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[3],
        is_equal_to(SOLENOID_PWM_ON));

    // turn brake lights off
    assert_that(
        g_mock_arduino_digital_write_pin,
        is_equal_to(PIN_BRAKE_LIGHT));

    assert_that(
        g_mock_arduino_digital_write_val,
        is_equal_to(LOW));

    // open master cylinder
    assert_that(
        g_mock_arduino_analog_write_pins[4],
        is_equal_to(PIN_MASTER_CYLINDER_SOLENOID));

    assert_that(
        g_mock_arduino_analog_write_val[4],
        is_equal_to(SOLENOID_PWM_OFF));

    // turn release solenoids off
    assert_that(
        g_mock_arduino_analog_write_pins[5],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_LEFT));

    assert_that(
        g_mock_arduino_analog_write_val[5],
        is_equal_to(SOLENOID_PWM_OFF));

    assert_that(
        g_mock_arduino_analog_write_pins[6],
        is_equal_to(PIN_RELEASE_SOLENOID_FRONT_RIGHT));

    assert_that(
        g_mock_arduino_analog_write_val[6],
        is_equal_to(SOLENOID_PWM_OFF));
}
