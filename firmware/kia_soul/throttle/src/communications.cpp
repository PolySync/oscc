/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "throttle_control.h"


static void process_throttle_command(
    const uint8_t * const data );

static void process_rx_frame(
    can_frame_s * const frame );


void publish_throttle_report( void )
{
    oscc_throttle_report_s throttle_report;

    throttle_report.enabled = (uint8_t) g_throttle_control_state.enabled;
    throttle_report.operator_override = (uint8_t) g_throttle_control_state.operator_override;
    throttle_report.dtcs = g_throttle_control_state.dtcs;

    g_control_can.sendMsgBuf(
        OSCC_THROTTLE_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_THROTTLE_REPORT_CAN_DLC,
        (uint8_t*) &throttle_report );
}


void publish_fault_report( void )
{
    oscc_fault_report_s fault_report;

    fault_report.fault_origin_id = FAULT_ORIGIN_THROTTLE;

    g_control_can.sendMsgBuf(
        OSCC_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
}


void check_for_controller_command_timeout( void )
{
    if( g_throttle_control_state.enabled == true )
    {
        if( g_throttle_command_timeout == true )
        {
            disable_control( );

            publish_fault_report( );

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


static void process_throttle_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_throttle_command_s * const throttle_command =
                (oscc_throttle_command_s *) data;

        if( throttle_command->enable == true )
        {
            enable_control( );

            DEBUG_PRINT("commanded spoof low: ");
            DEBUG_PRINT(throttle_command->spoof_value_low);
            DEBUG_PRINT(" high: ");
            DEBUG_PRINTLN(throttle_command->spoof_value_high);

            update_throttle(
                throttle_command->spoof_value_high,
                throttle_command->spoof_value_low );
        }
        else
        {
            disable_control( );
        }

        g_throttle_command_timeout = false;
    }
}


static void process_rx_frame(
    can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if( frame->id == OSCC_THROTTLE_COMMAND_CAN_ID )
        {
            process_throttle_command( frame->data );
        }
        else if ( frame->id == OSCC_FAULT_REPORT_CAN_ID )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
        }
    }
}
