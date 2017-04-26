#include "mcp_can.h"
#include "can.h"
#include "gateway_protocol_can.h"
#include "steering_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "steering_control.h"


void publish_steering_report( void )
{
    oscc_report_steering_s steering_report;

    steering_report.data.angle = steering_state.steering_angle;
    steering_report.data.override = (uint8_t) steering_control_state.operator_override;
    steering_report.data.angle_command = steering_state.steering_angle_target;
    steering_report.data.torque = torque_sum;
    steering_report.data.enabled = (uint8_t) steering_control_state.enabled;

    can.sendMsgBuf( steering_report.id,
                    CAN_STANDARD,
                    steering_report.dlc,
                    (uint8_t *) &steering_report.data );

    g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
}


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_steering_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_steering_report( );
    }
}


void process_steering_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_command_steering_data_s * const steering_command_data =
                (oscc_command_steering_data_s *) data;

        steering_state.steering_angle_target =
            (steering_command_data->steering_wheel_angle_command / 9.0);

        steering_state.steering_angle_rate_max =
            (steering_command_data->steering_wheel_max_velocity * 9.0);

        if ( (steering_command_data->enabled == 1)
            && (steering_control_state.enabled == false)
            && (steering_control_state.emergency_stop == false) )
        {
            enable_control( );
        }

        if ( (steering_command_data->enabled == 0)
            && (steering_control_state.enabled == true) )
        {
            disable_control( );
        }

        g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


void process_chassis_state_1_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_report_chassis_state_1_data_s * const chassis_state_1_data =
                (oscc_report_chassis_state_1_data_s *) data;

        float raw_angle = (float)chassis_state_1_data->steering_wheel_angle;
        steering_state.steering_angle = raw_angle * 0.0076294;

        // Convert from 40 degree range to 470 degree range in 1 degree increments
        steering_state.steering_angle *= 11.7;
    }
}


void process_rx_frame(
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


void check_for_command_timeout( void )
{
    bool timeout = is_timeout(
            g_steering_command_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            PARAM_COMMAND_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        if( steering_control_state.enabled == true )
        {
            disable_control( );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}
