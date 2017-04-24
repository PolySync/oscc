/**
* @file throttle_module_state.c
* @brief Throttle module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>

#include "macros.h"
#include "can_monitor.h"
#include "system_state.h"
#include "throttle_module_state.h"




// *****************************************************
// static definitions
// *****************************************************


//
static int analyze_command_frame(
        throttle_module_state_s * const state,
        const ps_ctrl_throttle_command_msg * const throttle_command )
{
    int module_state = STATE_OK;


    return module_state;
}


//
static int analyze_report_frame(
        throttle_module_state_s * const state,
        const ps_ctrl_throttle_report_msg * const throttle_report )
{
    int module_state = STATE_OK;

    state->control_state = throttle_report->enabled;

    state->override_triggered = throttle_report->override;

    if( throttle_report->fault_wdc == 1 ||
        throttle_report->fault_1 == 1 ||
        throttle_report->fault_2 == 1 ||
        throttle_report->fault_connector == 1 )
    {
        module_state = STATE_FAULT;
    }


    return module_state;
}




// *****************************************************
// public definitions
// *****************************************************


//
int analyze_throttle_state(
        throttle_module_state_s * const state,
        const can_frame_s * const throttle_command_frame,
        const can_frame_s * const throttle_report_frame )
{
    int ret = NOERR;

    analyze_command_frame(
            state,
            (ps_ctrl_throttle_command_msg*)
                    throttle_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame(
            state,
            (ps_ctrl_throttle_report_msg*)
                    throttle_report_frame->frame_contents.buffer );

    return ret;
}
