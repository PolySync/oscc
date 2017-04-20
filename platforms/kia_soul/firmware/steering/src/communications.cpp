#include "mcp_can.h"
#include "can.h"
#include "gateway_protocol_can.h"
#include "steering_protocol_can.h"
#include "time.h"
#include "debug.h"

#include "communications.h"
#include "steering_module.h"
#include "steering_control.h"


void publish_steering_report(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *report,
    MCP_CAN &can,
    uint8_t torque_sum )
{
    report->id = ( uint32_t ) ( OSCC_CAN_ID_STEERING_REPORT );

    report->dlc = 8;

    // Get a pointer to the data buffer in the CAN frame and set
    // the steering angle
    oscc_report_msg_steering * data =
        ( oscc_report_msg_steering* ) report->data;

    data->angle = steering_module->state.steering_angle;

    report->timestamp = millis( );

    // set override flag
    if ( ( steering_module->override_flags.wheel == 0 ) &&
            ( steering_module->override_flags.voltage == 0 ) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->angle_command = steering_module->state.steering_angle_target;

    data->torque = torque_sum;

    data->enabled = (uint8_t) steering_module->control_state.enabled;

    can.sendMsgBuf( report->id,
                    0,
                    report->dlc,
                    report->data );
}


void publish_timed_tx_frames(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *report,
    MCP_CAN &can,
    uint8_t torque_sum )
{
    uint32_t delta = get_time_delta( report->timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_PUBLISH_INTERVAL_STEERING_REPORT )
    {
        publish_steering_report( steering_module, report, can, torque_sum );
    }
}


void process_steering_command(
    kia_soul_steering_module_s *steering_module,
    const oscc_command_msg_steering * const control_data,
    can_frame_s *command,
    DAC_MCP49xx &dac )
{
    steering_module->state.steering_angle_target =
        control_data->steering_wheel_angle_command / 9.0;

    steering_module->params.steering_angle_rate_max =
        control_data->steering_wheel_max_velocity * 9.0;

    if ( ( control_data->enabled == 1 ) &&
            ( steering_module->control_state.enabled == false ) &&
            ( steering_module->control_state.emergency_stop == false ) )
    {
        enable_control( steering_module, dac );
    }

    if ( ( control_data->enabled == 0 ) &&
            ( steering_module->control_state.enabled == true ) )
    {
        disable_control( steering_module, dac );
    }

    command->timestamp = millis( );
}


void process_chassis_state1(
    kia_soul_steering_module_s *steering_module,
    const oscc_chassis_state1_data_s * const chassis_data )
{
    float raw_angle = (float)chassis_data->steering_wheel_angle;
    steering_module->state.steering_angle = raw_angle * 0.0076294;

    // Convert from 40 degree range to 470 degree range in 1 degree increments
    steering_module->state.steering_angle *= 11.7;
}


void handle_ready_rx_frames(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *frame,
    can_frame_s *command,
    DAC_MCP49xx &dac )
{
    if ( frame->id == OSCC_CAN_ID_STEERING_COMMAND )
    {
        process_steering_command(
            steering_module,
            ( const oscc_command_msg_steering * const )frame->data,
            command,
            dac);
    }
    else if ( frame->id == KIA_STATUS1_MESSAGE_ID )
    {
        process_chassis_state1(
            steering_module,
            ( const oscc_chassis_state1_data_s * const )frame->data );
    }
}

void check_rx_timeouts(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *command,
    DAC_MCP49xx &dac )
{
    bool timeout = is_timeout(
            command->timestamp,
            GET_TIMESTAMP_MS( ),
            steering_module->params.rx_timeout);

    if( timeout == true )
    {
        if( steering_module->control_state.enabled == true )
        {
            disable_control( steering_module, dac );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}