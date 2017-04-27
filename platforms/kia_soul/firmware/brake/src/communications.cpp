#include <Arduino.h>
#include "mcp_can.h"
#include "chassis_state_can_protocol.h"
#include "brake_protocol_can.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "brake_control.h"


void publish_brake_report( void )
{
    oscc_report_brake_s brake_report;

    brake_report.data.enabled = (uint8_t) brake_control_state.operator_override;
    brake_report.data.pedal_input = ( uint16_t )brake_state.can_pressure;
    brake_report.data.pedal_command = ( uint16_t )brake_state.pedal_command;
    brake_report.data.pedal_output = ( uint16_t )brake_state.current_pressure;

    can.sendMsgBuf(
        brake_report.id,
        CAN_STANDARD,
        brake_report.dlc,
        (uint8_t *) &brake_report.data );

    g_brake_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
}


void publish_reports( void )
{
    uint32_t delta = get_time_delta( g_brake_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_brake_report( );
    }
}


void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_command_brake_data_s * const brake_command_data =
                (oscc_command_brake_data_s *) data;

        if( (brake_command_data->enabled == 1)
            && (brake_control_state.enabled == false) )
        {
            brake_enable( );
        }

        if( (brake_command_data->enabled == 0)
            && (brake_control_state.enabled == true) )
        {
            brake_disable( );
        }

        brake_state.pedal_command = brake_command_data->pedal_command;

        g_brake_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


void process_chassis_state_1(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_report_chassis_state_1_data_s * const chassis_state_1_data =
                (oscc_report_chassis_state_1_data_s *) data;

        brake_state.can_pressure = chassis_state_1_data->brake_pressure;
    }
}


void process_rx_frame(
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


void check_for_command_timeout( void )
{
    bool timeout = is_timeout(
        g_brake_command_last_rx_timestamp,
        GET_TIMESTAMP_MS( ),
        PARAM_COMMAND_TIMEOUT_IN_MSEC );

    if ( timeout == true )
    {
        brake_disable( );
        DEBUG_PRINTLN( "Control disabled: Timeout" );
    }
}
