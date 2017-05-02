/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "can.h"
#include "throttle_can_protocol.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "throttle_control.h"


static void publish_throttle_report( void );

static void process_throttle_command(
    const uint8_t * const data );

static void process_rx_frame(
    can_frame_s * const frame );


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_throttle_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if( delta >= OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_throttle_report( );
    }
}


void check_for_controller_command_timeout( void )
{
    if( g_throttle_control_state.enabled == true )
    {
        bool timeout = is_timeout(
                g_throttle_command_last_rx_timestamp,
                GET_TIMESTAMP_MS( ),
                PARAM_COMMAND_TIMEOUT_IN_MSEC );

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
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}

static void publish_throttle_report( void )
{
    oscc_report_throttle_s throttle_report;

    accelerator_position_s accelerator_position;
    read_accelerator_position_sensor( &accelerator_position );

    throttle_report.id = OSCC_REPORT_THROTTLE_CAN_ID;
    throttle_report.dlc = OSCC_REPORT_THROTTLE_CAN_DLC;
    throttle_report.data.enabled = (uint8_t) g_throttle_control_state.enabled;
    throttle_report.data.override = (uint8_t) g_throttle_control_state.operator_override;
    throttle_report.data.accelerator_input = (accelerator_position.low + accelerator_position.high);
    throttle_report.data.accelerator_command = g_throttle_control_state.commanded_accelerator_position;

    g_control_can.sendMsgBuf(
            throttle_report.id,
            CAN_STANDARD,
            throttle_report.dlc,
            (uint8_t*) &throttle_report.data );

    g_throttle_report_last_tx_timestamp = GET_TIMESTAMP_MS();
}


static void process_throttle_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_command_throttle_data_s * const throttle_command_data =
                (oscc_command_throttle_data_s *) data;

        if( throttle_command_data->enabled == true )
        {
            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_throttle_control_state.commanded_accelerator_position =
            throttle_command_data->accelerator_command;

        DEBUG_PRINT( "controller commanded accelerator position: " );
        DEBUG_PRINTLN( g_throttle_control_state.commanded_accelerator_position );

        update_throttle( );

        g_throttle_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_rx_frame(
    can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if( frame->id == OSCC_COMMAND_THROTTLE_CAN_ID )
        {
            process_throttle_command( frame->data );
        }
    }
}
