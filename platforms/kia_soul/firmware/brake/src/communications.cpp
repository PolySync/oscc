/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "chassis_state_can_protocol.h"
#include "brake_can_protocol.h"
#include "oscc_can.h"
#include "oscc_time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "brake_control.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void publish_brake_report( void );

static void process_brake_command(
    const uint8_t * const data );

static void process_chassis_state_1(
    const uint8_t * const data );


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_brake_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_brake_report( );
    }
}


void check_for_controller_command_timeout( void )
{
    if( g_brake_control_state.enabled == true )
    {
        bool timeout = is_timeout(
            g_brake_command_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            COMMAND_TIMEOUT_IN_MSEC );

        if ( timeout == true )
        {
            brake_disable( );

            DEBUG_PRINTLN( "Timeout" );
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


static void publish_brake_report( void )
{
    oscc_report_brake_s brake_report;

    brake_report.id = OSCC_REPORT_BRAKE_CAN_ID;
    brake_report.dlc = OSCC_REPORT_BRAKE_CAN_DLC;
    brake_report.data.enabled = (uint8_t) g_brake_control_state.operator_override;
    brake_report.data.pedal_input = ( uint16_t )g_brake_control_state.current_vehicle_brake_pressure;
    brake_report.data.pedal_command = g_brake_control_state.commanded_pedal_position;
    brake_report.data.pedal_output = g_brake_control_state.current_sensor_brake_pressure;

    g_control_can.sendMsgBuf(
        brake_report.id,
        CAN_STANDARD,
        brake_report.dlc,
        (uint8_t *) &brake_report.data );

    g_brake_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
}


static void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_command_brake_data_s * const brake_command_data =
                (oscc_command_brake_data_s *) data;

        if( brake_command_data->enabled == true )
        {
            brake_enable( );
        }

        if( brake_command_data->enabled == false )
        {
            brake_disable( );
        }

        g_brake_control_state.commanded_pedal_position = brake_command_data->pedal_command;

        DEBUG_PRINT( "controller commanded brake pressure: " );
        DEBUG_PRINTLN( g_brake_control_state.commanded_pedal_position );

        brake_update( );

        g_brake_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_chassis_state_1(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_report_chassis_state_1_data_s * const chassis_state_1_data =
                (oscc_report_chassis_state_1_data_s *) data;

        g_brake_control_state.current_vehicle_brake_pressure =
            chassis_state_1_data->brake_pressure;
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
        else if ( frame->id == OSCC_REPORT_CHASSIS_STATE_1_CAN_ID )
        {
            process_chassis_state_1( frame->data );
        }
    }
}
