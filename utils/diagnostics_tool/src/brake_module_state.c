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
* @file brake_module_state.c
* @brief brake module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>

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
        const ps_ctrl_brake_command_msg * const brake_command )
{
    int module_state = STATE_OK;


    return module_state;
}


//
static int analyze_report_frame(
        brake_module_state_s * const state,
        const ps_ctrl_brake_report_msg * const brake_report )
{
    int module_state = STATE_OK;

    state->control_state = brake_report->enabled;

    state->override_triggered = brake_report->override;

    if( brake_report->fault_wdc == 1 ||
        brake_report->fault_1 == 1 ||
        brake_report->fault_2 == 1 ||
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
            (ps_ctrl_brake_command_msg*)
                    brake_command_frame->frame_contents.buffer ); // TODO : do we need this?

    state->module_state = analyze_report_frame( 
            state, 
            (ps_ctrl_brake_report_msg*) 
                    brake_report_frame->frame_contents.buffer );

    return ret;
}
