/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "brake_can_protocol.h"
#include "fault_can_protocol.h"
#include "oscc_can.h"
#include "oscc_time.h"
#include "debug.h"
#include "kia_soul.h"

#include "globals.h"
#include "communications.h"
#include "brake_control.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void process_brake_command(
    const uint8_t * const data );

static void process_obd_frame(
    const uint8_t * const data );

static void check_for_controller_command_timeout( void );

static void check_for_obd_timeout( void );


void publish_brake_report( void )
{
    uint32_t delta = get_time_delta( g_brake_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_BRAKE_REPORT_PUBLISH_INTERVAL_IN_MSEC )
    {
        oscc_brake_report_s brake_report;

        brake_report.enabled = (uint8_t) g_brake_control_state.enabled;
        brake_report.override = (uint8_t) g_brake_control_state.operator_override;
        brake_report.pedal_input = (int16_t) g_brake_control_state.current_vehicle_brake_pressure;
        brake_report.pedal_command = (uint16_t) g_brake_control_state.commanded_pedal_position;
        brake_report.pedal_output = (uint16_t) g_brake_control_state.current_sensor_brake_pressure;
        brake_report.fault_obd_timeout = (uint8_t) g_brake_control_state.obd_timeout;
        brake_report.fault_invalid_sensor_value = (uint8_t) g_brake_control_state.invalid_sensor_value;

        g_control_can.sendMsgBuf(
            OSCC_BRAKE_REPORT_CAN_ID,
            CAN_STANDARD,
            OSCC_BRAKE_REPORT_CAN_DLC,
            (uint8_t *) &brake_report );

        g_brake_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
    }
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


void check_for_can_frame( void )
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

    check_for_obd_timeout( );
}


static void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_brake_command_s * const brake_command =
                (oscc_brake_command_s *) data;

        if( brake_command->enabled == true )
        {
            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_brake_control_state.commanded_pedal_position = brake_command->pedal_command;

        g_brake_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_obd_brake_pressure(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const kia_soul_obd_brake_pressure_data_s * const brake_pressure_data =
                (kia_soul_obd_brake_pressure_data_s *) data;

        g_brake_control_state.current_vehicle_brake_pressure =
            brake_pressure_data->master_cylinder_pressure;

        g_obd_brake_pressure_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
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
        else if ( frame->id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID )
        {
            process_obd_brake_pressure( frame->data );
        }
        else if ( frame->id == OSCC_MODULE_FAULT_REPORT_CAN_ID )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
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

            publish_fault_report( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


static void check_for_obd_timeout( void )
{
    bool timeout = is_timeout(
            g_obd_brake_pressure_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            OBD_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        disable_control( );

        publish_fault_report( );

        g_brake_control_state.obd_timeout = true;

        DEBUG_PRINTLN( "Timeout - OBD - brake pressure" );
    }
    else
    {
        g_brake_control_state.obd_timeout = false;
    }
}
