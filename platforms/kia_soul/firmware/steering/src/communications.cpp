/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "chassis_state_can_protocol.h"
#include "steering_can_protocol.h"
#include "oscc_time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "steering_control.h"


static void publish_steering_report( void );

static void process_steering_command(
    const uint8_t * const data );

static void process_chassis_state_1_report(
    const uint8_t * const data );

static void process_rx_frame(
    can_frame_s * const frame );

static void check_for_controller_command_timeout( void );

static void check_for_chassis_state_1_report_timeout( void );


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_steering_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_steering_report( );
    }
}


void check_for_incoming_message( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


void check_for_timeouts( void )
{
    check_for_controller_command_timeout( );

    check_for_chassis_state_1_report_timeout( );
}


static void publish_steering_report( void )
{
    oscc_report_steering_s steering_report;

    steering_report.id = OSCC_REPORT_STEERING_CAN_ID;
    steering_report.dlc = OSCC_REPORT_STEERING_CAN_DLC;
    steering_report.data.enabled = (uint8_t) g_steering_control_state.enabled;
    steering_report.data.override = (uint8_t) g_steering_control_state.operator_override;
    steering_report.data.current_steering_wheel_angle = (int16_t) g_steering_control_state.current_steering_wheel_angle;
    steering_report.data.commanded_steering_wheel_angle = (int16_t) g_steering_control_state.commanded_steering_wheel_angle;
    steering_report.data.fault_obd_timeout = (uint8_t) g_steering_control_state.obd_timeout;
    steering_report.data.spoofed_torque_output = (int8_t) g_spoofed_torque_output_sum;
    steering_report.data.fault_invalid_sensor_value = (uint8_t) g_steering_control_state.invalid_sensor_value;

    g_control_can.sendMsgBuf(
        steering_report.id,
        CAN_STANDARD,
        steering_report.dlc,
        (uint8_t *) &steering_report.data );

    g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
}


static void process_steering_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_command_steering_data_s * const steering_command_data =
                (oscc_command_steering_data_s *) data;

        if ( steering_command_data->enabled == true )
        {
            // divisor values found empirically to best match steering output
            g_steering_control_state.commanded_steering_wheel_angle =
                (steering_command_data->commanded_steering_wheel_angle / 9.0);

            g_steering_control_state.commanded_steering_wheel_angle_rate =
                (steering_command_data->commanded_steering_wheel_angle_rate * 9.0);

            DEBUG_PRINT( "controller commanded steering wheel angle: " );
            DEBUG_PRINTLN( g_steering_control_state.commanded_steering_wheel_angle );

            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_chassis_state_1_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_report_chassis_state_1_data_s * const chassis_state_1_data =
                (oscc_report_chassis_state_1_data_s *) data;

        if( chassis_state_1_data->flags
            & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID )
        {
            g_steering_control_state.current_steering_wheel_angle =
                chassis_state_1_data->steering_wheel_angle
                * RAW_ANGLE_SCALAR
                * WHEEL_ANGLE_TO_STEERING_WHEEL_ANGLE_SCALAR;

            g_chassis_state_1_report_last_rx_timestamp = GET_TIMESTAMP_MS( );
        }
    }
}


static void process_rx_frame(
    can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if ( frame->id == OSCC_COMMAND_STEERING_CAN_ID )
        {
            process_steering_command( frame->data );
        }
        else if ( frame->id == OSCC_REPORT_CHASSIS_STATE_1_CAN_ID )
        {
            process_chassis_state_1_report( frame->data );
        }
    }
}


static void check_for_controller_command_timeout( void )
{
    if( g_steering_control_state.enabled == true )
    {
        bool timeout = is_timeout(
                g_steering_command_last_rx_timestamp,
                GET_TIMESTAMP_MS( ),
                COMMAND_TIMEOUT_IN_MSEC);

        if( timeout == true )
        {
            disable_control( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


static void check_for_chassis_state_1_report_timeout( void )
{
    bool timeout = is_timeout(
            g_chassis_state_1_report_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            CHASSIS_STATE_1_REPORT_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        disable_control( );

        g_steering_control_state.obd_timeout = true;

        DEBUG_PRINTLN( "Timeout - Chassis State 1 report" );
    }
    else
    {
        g_steering_control_state.obd_timeout = false;
    }
}
