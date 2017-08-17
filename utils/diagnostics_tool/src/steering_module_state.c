/**
* @file steering_module_state.c
* @brief Steering module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>
#include "can_protocols/steering_can_protocol.h"

#include "macros.h"
#include "can_monitor.h"
#include "system_state.h"
#include "steering_module_state.h"




// *****************************************************
// static declarations
// *****************************************************


//
static int analyze_command_frame(
        steering_module_state_s * const state,
        const oscc_steering_command_s * const steering_command )
{
    int module_state = STATE_OK;

    //state->control_state = steering_command->enabled;

    // TODO: analyze ignore and clear

    // TODO: analyze count

    // TODO: check steering_wheel_max_velocity, commanded_steering_wheel_angle, torque for valid ranges

    return module_state;
}


//
static int analyze_report_frame(
        steering_module_state_s * const state,
        const oscc_steering_report_s * const steering_report )
{
    int module_state = STATE_OK;

    state->control_state = steering_report->enabled;

    state->override_triggered = steering_report->operator_override;

    if( steering_report->dtcs != 0 )
    {
        module_state = STATE_FAULT;
    }

    // TODO: check driver activity?

    // TODO: check vehicle_speed, torque, angle_command, angle for valid ranges

    return module_state;
}




// *****************************************************
// declarations
// *****************************************************


//
int analyze_steering_state(
        steering_module_state_s * const state,
        const can_frame_s * const steering_command_frame,
        const can_frame_s * const steering_report_frame )
{
    int ret = NOERR;

    analyze_command_frame(
            state,
            (oscc_steering_command_s*)
                    steering_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame(
            state,
            (oscc_steering_report_s*)
                    steering_report_frame->frame_contents.buffer );

    return ret;
}
