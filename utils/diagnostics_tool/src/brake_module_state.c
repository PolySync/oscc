/**
* @file brake_module_state.c
* @brief brake module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>
#include "brake_can_protocol.h"

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
        const oscc_command_brake_data_s * const brake_command )
{
    int module_state = STATE_OK;


    return module_state;
}


//
static int analyze_report_frame(
        brake_module_state_s * const state,
        const oscc_report_brake_data_s * const brake_report )
{
    int module_state = STATE_OK;

    state->control_state = brake_report->enabled;

    state->override_triggered = brake_report->override;

    if( brake_report->fault_wdc == 1 ||
        brake_report->fault_obd_timeout == 1 ||
        brake_report->fault_brake == 1 ||
        brake_report->fault_connector == 1 )
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
            (oscc_command_brake_data_s*)
                    brake_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame(
            state,
            (oscc_report_brake_data_s*)
                    brake_report_frame->frame_contents.buffer );

    return ret;
}
