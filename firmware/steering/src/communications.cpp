/**
 * @file communications.cpp
 *
 */


#include <stdint.h>

#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "mcp_can.h"
#include "steering_control.h"
#include "oscc_can.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void process_steering_command(
    const uint8_t * const data );

static void process_fault_report(
    const uint8_t * const data );


void publish_steering_report( void )
{
    oscc_steering_report_s steering_report;

    steering_report.magic[0] = (uint8_t) OSCC_MAGIC_BYTE_0;
    steering_report.magic[1] = (uint8_t) OSCC_MAGIC_BYTE_1;
    steering_report.enabled = (uint8_t) g_steering_control_state.enabled;
    steering_report.operator_override = (uint8_t) g_steering_control_state.operator_override;
    steering_report.dtcs = g_steering_control_state.dtcs;

    cli();
    g_control_can.sendMsgBuf(
        OSCC_STEERING_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_STEERING_REPORT_CAN_DLC,
        (uint8_t *) &steering_report );
    sei();
}


void publish_fault_report( void )
{
    oscc_fault_report_s fault_report;

    fault_report.magic[0] = (uint8_t) OSCC_MAGIC_BYTE_0;
    fault_report.magic[1] = (uint8_t) OSCC_MAGIC_BYTE_1;
    fault_report.fault_origin_id = FAULT_ORIGIN_STEERING;
    fault_report.dtcs = g_steering_control_state.dtcs;

    cli();
    g_control_can.sendMsgBuf(
        OSCC_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
    sei();
}


void check_for_controller_command_timeout( void )
{
    if( g_steering_control_state.enabled == true )
    {
        if( g_steering_command_timeout == true )
        {
            disable_control( );

            publish_fault_report( );

            DEBUG_PRINTLN( "Timeout - controller command" );
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


static void process_rx_frame(
    const can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if( (frame->data[0] == OSCC_MAGIC_BYTE_0)
             && (frame->data[1] == OSCC_MAGIC_BYTE_1) )
        {
            if ( frame->id == OSCC_STEERING_ENABLE_CAN_ID )
            {
                enable_control( );
            }
            else if ( frame->id == OSCC_STEERING_DISABLE_CAN_ID )
            {
                disable_control( );
            }
            else if ( frame->id == OSCC_STEERING_COMMAND_CAN_ID )
            {
                process_steering_command( frame->data );
            }
            else if ( frame->id == OSCC_FAULT_REPORT_CAN_ID )
            {
                process_fault_report( frame->data );
            }
        }
    }
}


static void process_steering_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_steering_command_s * const steering_command =
                (oscc_steering_command_s *) data;

        update_steering(
            steering_command->spoof_value_high,
            steering_command->spoof_value_low );

        g_steering_command_timeout = false;
    }
}


static void process_fault_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_fault_report_s * const fault_report =
                (oscc_fault_report_s *) data;

        disable_control( );

        DEBUG_PRINT( "Fault report received from: " );
        DEBUG_PRINT( fault_report->fault_origin_id );
        DEBUG_PRINT( "  DTCs: ");
        DEBUG_PRINTLN( fault_report->dtcs );
    }
}
