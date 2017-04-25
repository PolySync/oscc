#include "mcp_can.h"
#include "can.h"
#include "gateway_protocol_can.h"
#include "steering_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "steering_module.h"
#include "steering_control.h"


void publish_steering_report( void )
{
    tx_frame_steering_report.id = ( uint32_t ) ( OSCC_CAN_ID_STEERING_REPORT );

    tx_frame_steering_report.dlc = 8;

    // Get a pointer to the data buffer in the CAN frame and set
    // the steering angle
    oscc_report_msg_steering * data =
        ( oscc_report_msg_steering* ) tx_frame_steering_report.data;

    data->angle = steering_state.steering_angle;

    tx_frame_steering_report.timestamp = GET_TIMESTAMP_MS( );

    // set override flag
    if ( steering_control_state.operator_override == false )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->angle_command = steering_state.steering_angle_target;

    data->torque = torque_sum;

    data->enabled = (uint8_t) steering_control_state.enabled;

    can.sendMsgBuf( tx_frame_steering_report.id,
                    0,
                    tx_frame_steering_report.dlc,
                    tx_frame_steering_report.data );
}


void publish_timed_tx_frames( void )
{
    uint32_t delta = get_time_delta( tx_frame_steering_report.timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_PUBLISH_INTERVAL_STEERING_REPORT )
    {
        publish_steering_report( );
    }
}


void process_steering_command(
    const oscc_command_msg_steering * const control_data )
{
    if ( control_data != NULL )
    {
        steering_state.steering_angle_target =
            control_data->steering_wheel_angle_command / 9.0;

        steering_state.steering_angle_rate_max =
            control_data->steering_wheel_max_velocity * 9.0;

        if ( ( control_data->enabled == 1 ) &&
                ( steering_control_state.enabled == false ) &&
                ( steering_control_state.emergency_stop == false ) )
        {
            enable_control( );
        }

        if ( ( control_data->enabled == 0 ) &&
                ( steering_control_state.enabled == true ) )
        {
            disable_control( );
        }

        rx_frame_steering_command.timestamp = GET_TIMESTAMP_MS( );
    }
}


void process_chassis_state1(
    const oscc_chassis_state1_data_s * const chassis_data )
{
    if ( chassis_data != NULL )
    {
        float raw_angle = (float)chassis_data->steering_wheel_angle;
        steering_state.steering_angle = raw_angle * 0.0076294;

        // Convert from 40 degree range to 470 degree range in 1 degree increments
        steering_state.steering_angle *= 11.7;
    }
}


void handle_ready_rx_frame(
    can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if ( frame->id == OSCC_CAN_ID_STEERING_COMMAND )
        {
            process_steering_command(
                ( const oscc_command_msg_steering * const )frame->data );
        }
        else if ( frame->id == KIA_STATUS1_MESSAGE_ID )
        {
            process_chassis_state1(
                ( const oscc_chassis_state1_data_s * const )frame->data );
        }
    }
}


void check_rx_timeouts( void )
{
    bool timeout = is_timeout(
            rx_frame_steering_command.timestamp,
            GET_TIMESTAMP_MS( ),
            PARAM_RX_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        if( steering_control_state.enabled == true )
        {
            disable_control( );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}