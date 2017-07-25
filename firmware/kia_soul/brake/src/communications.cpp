/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "oscc_can.h"
#include "debug.h"
#include "oscc.h"

#include "globals.h"
#include "communications.h"
#include "brake_control.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void process_brake_command(
    const uint8_t * const data );

static void process_fault_report(
    const uint8_t * const data );


void publish_brake_report( void )
{
    cli();

    oscc_brake_report_s brake_report;

    brake_report.magic = (uint16_t) OSCC_MAGIC;
    brake_report.enabled = (uint8_t) g_brake_control_state.enabled;
    brake_report.operator_override = (uint8_t) g_brake_control_state.operator_override;
    brake_report.dtcs = g_brake_control_state.dtcs;

    g_control_can.sendMsgBuf(
        OSCC_BRAKE_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_BRAKE_REPORT_CAN_DLC,
        (uint8_t *) &brake_report );

    sei();
}


void publish_fault_report( void )
{
    cli();

    oscc_fault_report_s fault_report;

    fault_report.magic = (uint16_t) OSCC_MAGIC;
    fault_report.fault_origin_id = FAULT_ORIGIN_BRAKE;

    g_control_can.sendMsgBuf(
        OSCC_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );

    sei();
}


void check_for_controller_command_timeout( void )
{
    if( g_brake_control_state.enabled == true )
    {
        if ( g_brake_command_timeout == true )
        {
            disable_control( );

            publish_fault_report( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


void check_for_incoming_message( void )
{
    cli();

    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }

    sei();
}


static void process_rx_frame(
    const can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if ( frame->id == OSCC_BRAKE_COMMAND_CAN_ID )
        {
            process_brake_command( frame->data );
        }
        else if ( frame->id == OSCC_FAULT_REPORT_CAN_ID )
        {
             process_fault_report( frame->data );
        }
    }
}


static void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_brake_command_s * const brake_command =
                (oscc_brake_command_s *) data;

        if( brake_command->magic == OSCC_MAGIC )
        {
            if( brake_command->enable == true )
            {
                enable_control( );
            }
            else
            {
                disable_control( );
            }

            g_brake_control_state.commanded_pedal_position =
                brake_command->pedal_command;

            g_brake_command_timeout = false;
        }
    }
}


static void process_fault_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_fault_report_s * const fault_report =
                (oscc_fault_report_s *) data;

        if( fault_report->magic == OSCC_MAGIC )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
        }
    }
}
