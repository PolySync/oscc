#include <Arduino.h>
#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "brake_protocol_can.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"


void publish_brake_report( void )
{
    oscc_report_msg_brake report;

    report.override = ( uint8_t )brake_control_state.driver_override;

    if ( brake_control_state.enabled == true )
    {
        report.enabled = 1;
    }
    else
    {
        report.enabled = 0;
    }

    report.pedal_input = ( uint16_t )brake_state.can_pressure;
    report.pedal_command = ( uint16_t )brake_state.pedal_command;
    report.pedal_output = ( uint16_t )brake_state.current_pressure;

    can.sendMsgBuf( OSCC_CAN_ID_BRAKE_REPORT, // CAN ID
                    0,                        // standard ID (not extended)
                    8,                        // dlc
                    (byte*)&report );        // brake report
}


void publish_timed_tx_frames( void )
{
    static uint32_t tx_timestamp = 0;

    uint32_t delta = get_time_delta( tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_PUBLISH_INTERVAL_BRAKE_REPORT )
    {
        tx_timestamp = GET_TIMESTAMP_MS( );

        publish_brake_report( );
    }
}


void process_brake_command(
    const oscc_command_msg_brake * const control_data )
{
    if ( control_data->enabled == 1 )
    {
        brake_control_state.enable_request = true;
    }

    if ( control_data->enabled == 0 )
    {
        brake_control_state.enable_request = false;
    }

    brake_state.pedal_command = control_data->pedal_command;
}


void process_chassis_state1(
    const oscc_chassis_state1_data_s * const chassis_data )
{
    brake_state.can_pressure = chassis_data->brake_pressure;
}


void handle_ready_rx_frames( const can_frame_s * const rx_frame )
{
    if ( rx_frame->id == OSCC_CAN_ID_BRAKE_COMMAND )
    {
        brake_control_state.rx_timestamp = GET_TIMESTAMP_MS( );

        process_brake_command(
            ( const oscc_command_msg_brake * const )rx_frame->data );
    }
    else if ( rx_frame->id == KIA_STATUS1_MESSAGE_ID )
    {
        process_chassis_state1(
            ( const oscc_chassis_state1_data_s * const )rx_frame->data );
    }
}


void check_rx_timeouts( void )
{
    uint32_t delta = get_time_delta( brake_control_state.rx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= PARAM_RX_TIMEOUT_IN_MSEC )
    {
        brake_control_state.enable_request = false;

        DEBUG_PRINTLN("timeout");
    }
}
