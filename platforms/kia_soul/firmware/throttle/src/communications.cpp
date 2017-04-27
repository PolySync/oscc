#include "mcp_can.h"
#include "chassis_state_can_protocol.h"
#include "throttle_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "throttle_control.h"


void publish_throttle_report( void )
{
    oscc_report_throttle_s throttle_report;

    throttle_report.data.override = (uint8_t) throttle_control_state.operator_override;

    throttle_report.data.enabled = (uint8_t) throttle_control_state.enabled;

    throttle_report.data.accelerator_input =
        (throttle_state.accel_pos_sensor_low + throttle_state.accel_pos_sensor_high);

    throttle_report.data.accelerator_command = throttle_state.accel_pos_target;

    control_can.sendMsgBuf(
            throttle_report.id,
            CAN_STANDARD,
            throttle_report.dlc,
            (uint8_t*) &throttle_report.data );

    g_throttle_report_last_tx_timestamp = GET_TIMESTAMP_MS();
}


void publish_timed_report( void )
{
    uint32_t delta = get_time_delta( g_throttle_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if( delta >= OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_throttle_report( );
    }
}


void process_throttle_command(
        const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_command_throttle_data_s * const throttle_command_data =
                (oscc_command_throttle_data_s *) data;

        if( (throttle_command_data->enabled == 1)
            && (throttle_control_state.enabled == false)
            && (throttle_control_state.emergency_stop == false) )
        {
            enable_control( );
        }

        if( (throttle_command_data->enabled == 0)
            && (throttle_control_state.enabled == true) )
        {
            disable_control( );
        }

        throttle_state.accel_pos_target = throttle_command_data->accelerator_command / 24;

        DEBUG_PRINT( "accelerator position target: " );
        DEBUG_PRINTLN( throttle_state.accel_pos_target );

        g_throttle_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


void process_rx_frame( can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if( frame->id == OSCC_COMMAND_THROTTLE_CAN_ID )
        {
            process_throttle_command( frame->data );
        }
    }
}


void check_for_command_timeout( void )
{
    bool timeout = is_timeout(
            g_throttle_command_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            PARAM_COMMAND_TIMEOUT_IN_MSEC );

    if( timeout == true )
    {
        if( throttle_control_state.enabled == true )
        {
            disable_control( );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}
