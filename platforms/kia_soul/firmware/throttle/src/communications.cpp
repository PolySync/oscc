#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "throttle_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "throttle_module.h"
#include "throttle_control.h"


void publish_throttle_report( )
{
    // cast data
    oscc_report_msg_throttle * const data =
            (oscc_report_msg_throttle*) tx_frame_throttle_report.data;

    tx_frame_throttle_report.id = OSCC_CAN_ID_THROTTLE_REPORT;
    tx_frame_throttle_report.dlc = 8;

    if ( (override_flags.accelerator_pressed == false )
        && (override_flags.voltage == 0) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->enabled = (uint8_t) control_state.enabled;

    data->accelerator_input = throttle_state.accel_pos_sensor_low + throttle_state.accel_pos_sensor_high;

    // Set Pedal Command (PC)
    data->accelerator_command = throttle_state.accel_pos_target;

    // publish to control CAN bus
    can.sendMsgBuf(
            tx_frame_throttle_report.id,
            0, // standard ID (not extended)
            tx_frame_throttle_report.dlc,
            tx_frame_throttle_report.data );

    tx_frame_throttle_report.timestamp = GET_TIMESTAMP_MS();
}


void publish_timed_report( )
{
    uint32_t delta = get_time_delta( tx_frame_throttle_report.timestamp, GET_TIMESTAMP_MS() );

    if( delta >= OSCC_PUBLISH_INTERVAL_THROTTLE_REPORT )
    {
        publish_throttle_report( );
    }
}


void process_throttle_command(
        const uint8_t * const rx_frame_buffer )
{
    const oscc_command_msg_throttle * const control_data =
            (oscc_command_msg_throttle*) rx_frame_buffer;

    if( (control_data->enabled == 1)
        && (control_state.enabled == false)
        && (control_state.emergency_stop == false) )
    {
        enable_control( );
    }

    if( (control_data->enabled == 0)
        && (control_state.enabled == true) )
    {
        disable_control( );
    }

    rx_frame_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    throttle_state.accel_pos_target = control_data->accelerator_command / 24;

    DEBUG_PRINT( "accelerator position target: " );
    DEBUG_PRINTLN( throttle_state.accel_pos_target );
}


void handle_ready_rx_frame( can_frame_s *frame )
{
    // check for a supported frame ID
    if( frame->id == OSCC_CAN_ID_THROTTLE_COMMAND )
    {
        // process status1
        process_throttle_command( rx_frame_throttle_command.data );
    }
}


void check_rx_timeouts( )
{
    bool timeout = is_timeout(
            rx_frame_throttle_command.timestamp,
            GET_TIMESTAMP_MS( ),
            PARAM_RX_TIMEOUT );

    if( timeout == true )
    {
        if( control_state.enabled == true )
        {
            disable_control( );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}
