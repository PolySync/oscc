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

static void check_for_controller_command_timeout( void );

static void check_for_chassis_state_1_report_timeout( void );


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_brake_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_brake_report( );
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


void check_for_timeouts( void )
{
    check_for_controller_command_timeout( );

    check_for_chassis_state_1_report_timeout( );
}


static void publish_brake_report( void )
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
            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_brake_control_state.commanded_pedal_position = brake_command_data->pedal_command;

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

        if( chassis_state_1_data->flags
            & OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID )
        {
            g_brake_control_state.current_vehicle_brake_pressure =
                chassis_state_1_data->brake_pressure;

            g_chassis_state_1_report_last_rx_timestamp = GET_TIMESTAMP_MS( );
        }
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


static void check_for_controller_command_timeout( void )
{
    if( g_brake_control_state.enabled == true )
    {
        bool timeout = is_timeout(
            g_brake_command_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            COMMAND_TIMEOUT_IN_MSEC );

        if ( timeout == true )
        {
            disable_control( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


static void check_for_chassis_state_1_report_timeout( void )
{
    bool timeout = is_timeout(
            g_chassis_state_1_report_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            CHASSIS_STATE_1_REPORT_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        disable_control( );

        g_brake_control_state.obd_timeout = true;

        DEBUG_PRINTLN( "Timeout - Chassis State 1 report" );
    }
    else
    {
        g_brake_control_state.obd_timeout = false;
    }
}
