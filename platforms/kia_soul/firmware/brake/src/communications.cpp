/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "brake_can_protocol.h"
#include "fault_can_protocol.h"
#include "oscc_can.h"
#include "debug.h"
#include "kia_soul.h"

#include "globals.h"
#include "communications.h"
#include "brake_control.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void process_brake_command(
    const uint8_t * const data );


void publish_brake_report( void )
{
    oscc_report_brake_s brake_report;

    brake_report.id = OSCC_REPORT_BRAKE_CAN_ID;
    brake_report.dlc = OSCC_REPORT_BRAKE_CAN_DLC;
    brake_report.data.enabled = (uint8_t) g_brake_control_state.enabled;
    brake_report.data.override = (uint8_t) g_brake_control_state.operator_override;
    brake_report.data.pedal_input = (int16_t) g_brake_control_state.current_vehicle_brake_pressure;
    brake_report.data.pedal_command = (uint16_t) g_brake_control_state.commanded_pedal_position;
    brake_report.data.pedal_output = (uint16_t) g_brake_control_state.current_sensor_brake_pressure;
    brake_report.data.fault_obd_timeout = (uint8_t) g_brake_control_state.obd_timeout;
    brake_report.data.fault_invalid_sensor_value = (uint8_t) g_brake_control_state.invalid_sensor_value;

    g_control_can.sendMsgBuf(
        brake_report.id,
        CAN_STANDARD,
        brake_report.dlc,
        (uint8_t *) &brake_report.data );
}


void publish_fault_report( void )
{
    oscc_module_fault_report_s fault_report;

    fault_report.fault_origin_id = FAULT_ORIGIN_BRAKE;

    g_control_can.sendMsgBuf(
        OSCC_MODULE_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_MODULE_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
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
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_command_brake_data_s * const brake_command_data =
                (oscc_command_brake_data_s *) data;

        if( brake_command_data->enabled == true )
        {
            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_brake_control_state.commanded_pedal_position = brake_command_data->pedal_command;

        g_brake_command_timeout = false;
    }
}


static void process_rx_frame(
    const can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if ( frame->id == OSCC_COMMAND_BRAKE_CAN_ID )
        {
            process_brake_command( frame->data );
        }
        else if ( frame->id == OSCC_MODULE_FAULT_REPORT_CAN_ID )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
        }
    }
}
