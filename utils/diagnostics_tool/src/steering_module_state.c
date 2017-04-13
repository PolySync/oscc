/************************************************************************/
/* The MIT License (MIT)                                                */
/* =====================                                                */
/*                                                                      */
/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the “Software”), to deal in the Software without              */
/* restriction, including without limitation the rights to use,         */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell    */
/* copies of the Software, and to permit persons to whom the            */
/* Software is furnished to do so, subject to the following             */
/* conditions:                                                          */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        */
/* OTHER DEALINGS IN THE SOFTWARE.                                      */
/************************************************************************/


/**
* @file steering_module_state.c
* @brief Steering module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>

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
        const ps_ctrl_steering_command_msg * const steering_command )
{
    int module_state = STATE_OK;

    //state->control_state = steering_command->enabled;

    // TODO: analyze ignore and clear

    // TODO: analyze count

    // TODO: check steering_wheel_max_velocity, steering_wheel_angle_command, torque for valid ranges

    return module_state;
}


//
static int analyze_report_frame(
        steering_module_state_s * const state,
        const ps_ctrl_steering_report_msg * const steering_report )
{
    int module_state = STATE_OK;

    state->control_state = steering_report->enabled;

    state->override_triggered = steering_report->override;

    if( steering_report->fault_wdc == 1 ||
        steering_report->fault_1 == 1 ||
        steering_report->fault_2 == 1 ||
        steering_report->fault_calibration == 1 ||
        steering_report->fault_connector == 1 )
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
            (ps_ctrl_steering_command_msg*)
                    steering_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame( 
            state, 
            (ps_ctrl_steering_report_msg*)
                    steering_report_frame->frame_contents.buffer );

    return ret;
}
