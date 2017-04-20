#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "throttle_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "communications.h"
#include "throttle_module.h"
#include "throttle_control.h"


void publish_ps_ctrl_throttle_report(
    kia_soul_throttle_module_s *throttle_module,
    can_frame_s *report,
    MCP_CAN &can )
{
    // cast data
    ps_ctrl_throttle_report_msg * const data =
            (ps_ctrl_throttle_report_msg*) report->data;

    report->id = OSCC_CAN_ID_THROTTLE_REPORT;
    report->dlc = 8;

    if ( (throttle_module->override_flags.pedal == 0 )
        && (throttle_module->override_flags.voltage == 0) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->enabled = (uint8_t) throttle_module->control_state.enabled;

    data->pedal_input = throttle_module->state.accel_position_sensor_low + throttle_module->state.accel_position_sensor_high;

    // Set Pedal Command (PC)
    data->pedal_command = throttle_module->state.accel_position_target;

    // publish to control CAN bus
    can.sendMsgBuf(
            report->id,
            0, // standard ID (not extended)
            report->dlc,
            report->data );

    report->timestamp = GET_TIMESTAMP_MS();
}

void publish_timed_report(
    kia_soul_throttle_module_s *throttle_module,
    can_frame_s *report,
    MCP_CAN &can )
{
    uint32_t delta = get_time_delta( report->timestamp, GET_TIMESTAMP_MS() );

    if( delta >= OSCC_PUBLISH_INTERVAL_THROTTLE_REPORT )
    {
        publish_ps_ctrl_throttle_report(
            throttle_module,
            report,
            can );
    }
}

void process_ps_ctrl_throttle_command(
    kia_soul_throttle_module_s *throttle_module,
    const uint8_t * const rx_frame_buffer,
    can_frame_s *rx_frame_ps_ctrl_throttle_command,
    DAC_MCP49xx &dac )
{
    const ps_ctrl_throttle_command_msg * const control_data =
            (ps_ctrl_throttle_command_msg*) rx_frame_buffer;

    if( (control_data->enabled == 1)
        && (throttle_module->control_state.enabled == false)
        && (throttle_module->control_state.emergency_stop == false) )
    {
        enable_control( throttle_module, dac );
    }

    if( (control_data->enabled == 0)
        && (throttle_module->control_state.enabled == true) )
    {
        disable_control( throttle_module, dac );
    }

    rx_frame_ps_ctrl_throttle_command->timestamp = GET_TIMESTAMP_MS( );

    throttle_module->state.accel_position_target = control_data->pedal_command / 24;

    DEBUG_PRINT( "accelerator position target: " );
    DEBUG_PRINTLN( throttle_module->state.accel_position_target );
}

void handle_ready_rx_frames(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *frame,
        can_frame_s *rx_frame_ps_ctrl_throttle_command,
        DAC_MCP49xx &dac )
{
    // check for a supported frame ID
    if( frame->id == OSCC_CAN_ID_THROTTLE_COMMAND )
    {
        // process status1
        process_ps_ctrl_throttle_command( throttle_module, frame->data, rx_frame_ps_ctrl_throttle_command, dac );
    }
}

void check_rx_timeouts(
        kia_soul_throttle_module_s *throttle_module,
        can_frame_s *rx_frame,
        DAC_MCP49xx &dac )
{
    bool timeout = is_timeout(
            rx_frame->timestamp,
            GET_TIMESTAMP_MS( ),
            throttle_module->params.rx_timeout);

    if( timeout == true )
    {
        if( throttle_module->control_state.enabled == true )
        {
            disable_control( throttle_module, dac );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}
