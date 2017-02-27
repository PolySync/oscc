/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */

/* Permission is hereby granted, free of charge, to any person */
/* obtaining a copy of this software and associated documentation */
/* files (the “Software”), to deal in the Software without */
/* restriction, including without limitation the rights to use, */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following */
/* conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR */
/* OTHER DEALINGS IN THE SOFTWARE. */
/************************************************************************/


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
        const can_frame_s * const throttle_command_frame )
{
    int module_state = STATE_OK;

    ps_ctrl_throttle_command_msg * throttle_command =
            (ps_ctrl_throttle_command_msg*)
                    throttle_command_frame->frame_contents.buffer;



    return module_state;
}


//
static int analyze_report_frame(
        throttle_module_state_s * const state,
        const can_frame_s * const throttle_report_frame )
{
    int module_state = STATE_OK;

    ps_ctrl_throttle_report_msg * throttle_report =
            (ps_ctrl_throttle_report_msg*)
                    throttle_report_frame->frame_contents.buffer;

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

    analyze_command_frame( state, throttle_command_frame ); // TODO : do we need this?

    state->module_state = analyze_report_frame( state, throttle_report_frame );

    return ret;
}
