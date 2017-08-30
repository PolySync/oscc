#include <cgreen/cgreen.h>
#include <cgreen/mocks.h>
#include <cucumber-cpp/autodetect.hpp>
#include <sys/timeb.h>
#include <unistd.h>

#include "Arduino.h"
#include "communications.h"
#include "oscc_can.h"
#include "mcp_can.h"
#include "obd_can_protocol.h"
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

extern oscc_report_heartbeat_s g_tx_heartbeat;
extern oscc_report_chassis_state_1_s g_tx_chassis_state_1;
extern oscc_report_chassis_state_2_s g_tx_chassis_state_2;
extern oscc_report_chassis_state_3_s g_tx_chassis_state_3;

extern uint32_t g_obd_steering_wheel_angle_rx_timestamp;
extern uint32_t g_obd_wheel_speed_rx_timestamp;
extern uint32_t g_obd_brake_pressure_rx_timestamp;
extern uint32_t g_obd_turn_signal_rx_timestamp;
extern uint32_t g_obd_accelerator_pedal_position_rx_timestamp;
extern uint32_t g_obd_engine_rpm_temp_rx_timestamp;
extern uint32_t g_obd_vehicle_speed_rx_timestamp;
extern uint32_t g_obd_gear_position_rx_timestamp;


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

    g_mock_arduino_digital_write_pin = UINT8_MAX;
    g_mock_arduino_digital_write_val = UINT8_MAX;
    g_mock_arduino_analog_read_return = INT_MAX;

    memset(
        &g_tx_heartbeat,
        0,
        sizeof(g_tx_heartbeat));

    memset(
        &g_tx_chassis_state_1,
        0,
        sizeof(g_tx_chassis_state_1));

    memset(
        &g_tx_chassis_state_2,
        0,
        sizeof(g_tx_chassis_state_2));

    memset(
        &g_tx_chassis_state_3,
        0,
        sizeof(g_tx_chassis_state_3));

    g_obd_steering_wheel_angle_rx_timestamp = 0;
    g_obd_wheel_speed_rx_timestamp = 0;
    g_obd_brake_pressure_rx_timestamp = 0;
    g_obd_turn_signal_rx_timestamp = 0;
    g_obd_engine_rpm_temp_rx_timestamp = 0;
    g_obd_vehicle_speed_rx_timestamp = 0;
    g_obd_gear_position_rx_timestamp = 0;
    g_obd_accelerator_pedal_position_rx_timestamp = 0;

}


THEN("^the steering wheel angle heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT));
    }
}


THEN("^the steering wheel angle valid flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID));
    }
    else if(action == "cleared")
    {

        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID));
    }
}


THEN("^the steering wheel angle rate valid flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID));
    }
}


THEN("^the wheel speed heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT));
    }
}


THEN("^the wheel speed valid flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID));
    }
}


THEN("^the brake pressure heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT));
    }
}


THEN("^the brake pressure valid flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID));
    }
}


THEN("^the turn signal heartbeat warning should be (.*)")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT));
    }
}


THEN("^the left turn signal flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON));
    }
}


THEN("^the right turn signal flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON));
    }
}


THEN("^the brake signal flag should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON,
            is_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_chassis_state_1.data.flags & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON,
            is_not_equal_to(OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON));
    }
}

THEN("^the engine report heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_ENGINE_RPM_TEMP_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_ENGINE_RPM_TEMP_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_ENGINE_RPM_TEMP_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_ENGINE_RPM_TEMP_WARNING_BIT));
    }
}

THEN("^the vehicle speed heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_VEHICLE_SPEED_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_VEHICLE_SPEED_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_VEHICLE_SPEED_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_VEHICLE_SPEED_WARNING_BIT));
    }
}

THEN("^the gear position heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_GEAR_POSITION_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_GEAR_POSITION_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_GEAR_POSITION_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_GEAR_POSITION_WARNING_BIT));
    }
}

THEN("^the accelerator pedal position heartbeat warning should be (.*)$")
{
    REGEX_PARAM(std::string, action);

    if(action == "set")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_ACCELERATOR_POSITION_WARNING_BIT,
            is_equal_to(KIA_SOUL_OBD_ACCELERATOR_POSITION_WARNING_BIT));
    }
    else if(action == "cleared")
    {
        assert_that(
            g_tx_heartbeat.data.warning_register & KIA_SOUL_OBD_ACCELERATOR_POSITION_WARNING_BIT,
            is_not_equal_to(KIA_SOUL_OBD_ACCELERATOR_POSITION_WARNING_BIT));
    }
}

