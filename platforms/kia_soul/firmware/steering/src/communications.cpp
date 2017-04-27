#include "mcp_can.h"
#include "can.h"
#include "chassis_state_can_protocol.h"
#include "steering_can_protocol.h"
#include "time.h"
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


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_steering_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_steering_report( );
    }
}


void check_for_controller_command_timeout( void )
{
    if( steering_control_state.enabled == true )
    {
        bool timeout = is_timeout(
                g_steering_command_last_rx_timestamp,
                GET_TIMESTAMP_MS( ),
                PARAM_COMMAND_TIMEOUT_IN_MSEC);

        if( timeout == true )
        {
            disable_control( );

            DEBUG_PRINTLN( "Timeout waiting for controller command" );
        }
    }
}


void check_for_incoming_message( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


static void publish_steering_report( void )
{
    oscc_report_steering_s steering_report;

    steering_report.data.angle = steering_control_state.steering_angle;
    steering_report.data.override = (uint8_t) steering_control_state.operator_override;
    steering_report.data.angle_command = steering_control_state.commanded_steering_angle;
    steering_report.data.torque = torque_sum;
    steering_report.data.enabled = (uint8_t) steering_control_state.enabled;

    control_can.sendMsgBuf(
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
            enable_control( );
        }
        else
        {
            disable_control( );
        }

        steering_control_state.commanded_steering_angle =
            (steering_command_data->steering_wheel_angle_command / 9.0);

        DEBUG_PRINT( "controller commanded steering wheel angle: " );
        DEBUG_PRINTLN( steering_control_state.commanded_steering_angle );

        update_steering( );

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

        float raw_angle = (float)chassis_state_1_data->steering_wheel_angle;
        steering_control_state.steering_angle = raw_angle * 0.0076294;

        // Convert from 40 degree range to 470 degree range in 1 degree increments
        steering_control_state.steering_angle *= 11.7;
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
