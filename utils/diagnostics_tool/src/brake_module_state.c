/**
* @file brake_module_state.c
* @brief brake module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>
#include "can_protocols/brake_can_protocol.h"

#include "macros.h"
#include "can_monitor.h"
#include "system_state.h"
#include "brake_module_state.h"




// *****************************************************
// static definitions
// *****************************************************


//
static int analyze_command_frame(
        brake_module_state_s * const state,
        const oscc_brake_command_s * const brake_command )
{
    int module_state = STATE_OK;


    return module_state;
}


//
static int analyze_report_frame(
        brake_module_state_s * const state,
        const oscc_brake_report_s * const brake_report )
{
    int module_state = STATE_OK;

    state->control_state = brake_report->enabled;

    state->override_triggered = brake_report->operator_override;

    if( brake_report->dtcs != 0 )
    {
        module_state = STATE_FAULT;
    }


    return module_state;
}




// *****************************************************
// public definitions
// *****************************************************


//
int analyze_brake_state(
        brake_module_state_s * const state,
        const can_frame_s * const brake_command_frame,
        const can_frame_s * const brake_report_frame )
{
    int ret = NOERR;

    analyze_command_frame(
            state,
            (oscc_brake_command_s*)
                    brake_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame(
            state,
            (oscc_brake_report_s*)
                    brake_report_frame->frame_contents.buffer );

    return ret;
}
