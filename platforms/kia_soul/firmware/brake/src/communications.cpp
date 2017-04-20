#include <Arduino.h>
#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "brake_protocol_can.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "brake_module.h"
#include "communications.h"


void publish_ps_ctrl_brake_report( kia_soul_brake_module_s *brake_module, MCP_CAN &can )
{
    ps_ctrl_brake_report_msg report;

    report.override = ( uint8_t )brake_module->control_state.driver_override;

    if ( brake_module->control_state.enabled == true )
    {
        report.enabled = 1;
    }
    else
    {
        report.enabled = 0;
    }

    report.pedal_input = ( uint16_t )brake_module->state.can_pressure;
    report.pedal_command = ( uint16_t )brake_module->state.pedal_command;
    report.pedal_output = ( uint16_t )brake_module->state.current_pressure;

    can.sendMsgBuf( PS_CTRL_MSG_ID_BRAKE_REPORT, // CAN ID
                    0,                           // standard ID (not extended)
                    8,                           // dlc
                    (byte*)&report );            // brake report
}


void publish_timed_tx_frames( kia_soul_brake_module_s *brake_module, MCP_CAN &can )
{
    static uint32_t tx_timestamp = 0;

    uint32_t delta = get_time_delta( tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL )
    {
        tx_timestamp = GET_TIMESTAMP_MS( );

        publish_ps_ctrl_brake_report( brake_module, can );
    }
}


void process_ps_ctrl_brake_command(
    kia_soul_brake_module_s *brake_module,
    const ps_ctrl_brake_command_msg * const control_data )
{
    if ( control_data->enabled == 1 )
    {
        brake_module->control_state.enable_request = true;
    }

    if ( control_data->enabled == 0 )
    {
        brake_module->control_state.enable_request = false;
    }

    brake_module->state.pedal_command = control_data->pedal_command;
}


void process_psvc_chassis_state1(
    kia_soul_brake_module_s *brake_module,
    const psvc_chassis_state1_data_s * const chassis_data )
{
    brake_module->state.can_pressure = chassis_data->brake_pressure;
}


void handle_ready_rx_frames(
    kia_soul_brake_module_s *brake_module,
    can_frame_s *frame )
{
    if ( frame->id == PS_CTRL_MSG_ID_BRAKE_COMMAND )
    {
        brake_module->control_state.rx_timestamp = millis( );

        process_ps_ctrl_brake_command(
            brake_module,
            ( const ps_ctrl_brake_command_msg * const )frame->data );
    }

    if ( frame->id == KIA_STATUS1_MESSAGE_ID )
    {
        process_psvc_chassis_state1(
            brake_module,
            ( const psvc_chassis_state1_data_s * const )frame->data );
    }
}


void check_rx_timeouts(
    kia_soul_brake_module_s *brake_module )
{
    uint32_t delta = get_time_delta( brake_module->control_state.rx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= brake_module->params.rx_timeout )
    {
        brake_module->control_state.enable_request = false;

        DEBUG_PRINTLN("timeout");
    }
}
