/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "fault_can_protocol.h"
#include "steering_can_protocol.h"
#include "oscc_time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "steering_control.h"


static void process_steering_command(
    const uint8_t * const data );

static void process_rx_frame(
    can_frame_s * const frame );


void publish_steering_report( void )
{
    uint32_t delta = get_time_delta( g_steering_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC )
    {
        oscc_steering_report_s steering_report;

        steering_report.enabled = (uint8_t) g_steering_control_state.enabled;
        steering_report.operator_override = (uint8_t) g_steering_control_state.operator_override;
        steering_report.dtcs = g_steering_control_state.dtcs;

        g_control_can.sendMsgBuf(
            OSCC_REPORT_STEERING_CAN_ID,
            CAN_STANDARD,
            OSCC_REPORT_STEERING_CAN_DLC,
            (uint8_t *) &steering_report );

        g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
    }
}


void publish_fault_report( void )
{
    oscc_module_fault_report_s fault_report;

    fault_report.fault_origin_id = FAULT_ORIGIN_STEERING;

    g_control_can.sendMsgBuf(
        OSCC_MODULE_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_MODULE_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
}


void check_for_controller_command_timeout( void )
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


static void process_steering_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_steering_command_s * const steering_command =
                (oscc_steering_command_s *) data;

        if ( steering_command->enable == true )
        {
            enable_control( );

            DEBUG_PRINT("commanded spoof low: ");
            DEBUG_PRINT(steering_command->spoof_value_low);
            DEBUG_PRINT(" high: ");
            DEBUG_PRINTLN(steering_command->spoof_value_high);

            update_steering(
                steering_command->spoof_value_high,
                steering_command->spoof_value_low );
        }
        else
        {
            disable_control( );
        }

        g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
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
        else if ( frame->id == OSCC_MODULE_FAULT_REPORT_CAN_ID )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
        }
    }
}