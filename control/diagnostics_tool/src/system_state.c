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
* @file system_state.c
* @brief System State Source.
*
*/




#include <stdlib.h>
#include <stdio.h>

#include "macros.h"
#include "can_monitor.h"
#include "terminal_print.h"
#include "system_state.h"




// *****************************************************
// static global data
// *****************************************************


static current_system_state_s system_state;




// *****************************************************
// static definitions
// *****************************************************


//
static int update_steering_state()
{
    int ret = ERROR;

    const can_frame_s * const steering_command_frame =
            get_can_msg_array_index_reference( PS_CTRL_MSG_ID_STEERING_COMMAND );

    const can_frame_s * const steering_report_frame =
            get_can_msg_array_index_reference( PS_CTRL_MSG_ID_STEERING_REPORT );

    if( steering_command_frame != NULL  ||  steering_report_frame != NULL )
    {
        ret = analyze_steering_state(
                &system_state.steering_module_state,
                steering_command_frame,
                steering_report_frame );
    }
    
    return ret;
}


//
static int update_throttle_state()
{
    int ret = ERROR;

    const can_frame_s * const throttle_command_frame =
            get_can_msg_array_index_reference( PS_CTRL_THROTTLE_COMMAND_ID );

    const can_frame_s * const throttle_report_frame =
            get_can_msg_array_index_reference( PS_CTRL_MSG_ID_THROTTLE_REPORT );

    if( throttle_command_frame != NULL || throttle_report_frame != NULL )
    {
        ret = analyze_throttle_state(
                &system_state.throttle_module_state,
                throttle_command_frame,
                throttle_report_frame );
    }

    return ret;
}


//
static int update_brake_state()
{
    int ret = ERROR;

    const can_frame_s * const brake_command_frame =
            get_can_msg_array_index_reference( PS_CTRL_MSG_ID_BRAKE_COMMAND );

    const can_frame_s * const brake_report_frame =
            get_can_msg_array_index_reference( PS_CTRL_MSG_ID_BRAKE_REPORT );

    if( brake_command_frame != NULL  ||  brake_report_frame != NULL )
    {
        ret = analyze_brake_state(
                &system_state.brake_module_state,
                brake_command_frame,
                brake_report_frame );
    }

    return ret;
}


//
static int update_gateway_state()
{
    int ret = ERROR;

    const can_frame_s * const heartbeat_msg_frame =
            get_can_msg_array_index_reference( PSVC_HEARTBEAT_MSG_BASE_ID );

    const can_frame_s * const chassis_state1_frame =
            get_can_msg_array_index_reference( PSVC_CHASSIS_STATE1_MSG_ID );

    const can_frame_s * const chassis_state2_frame =
            get_can_msg_array_index_reference( PSVC_CHASSIS_STATE2_MSG_ID );

    if( heartbeat_msg_frame != NULL ||
            chassis_state1_frame != NULL ||
            chassis_state2_frame != NULL )
    {
        ret = analyze_gateway_state(
                &system_state.gateway_module_state,
                heartbeat_msg_frame,
                chassis_state1_frame,
                chassis_state2_frame );
    }

    return ret;
}




// *****************************************************
// public definitions
// *****************************************************


//
int update_system_state()
{
    int ret = NOERR;

    if( update_steering_state() == ERROR )
    {
        system_state.steering_module_state.module_state = STATE_FAULT;

        ret = ERROR;
    }

    if( update_throttle_state() == ERROR )
    {
        system_state.throttle_module_state.module_state = STATE_FAULT;

        ret = ERROR;
    }

    if( update_brake_state() == ERROR )
    {
        system_state.brake_module_state.module_state = STATE_FAULT;

        ret = ERROR;
    }

    if( update_gateway_state() == ERROR )
    {
        system_state.gateway_module_state.module_state = STATE_FAULT;

        ret = ERROR;
    }

    // TODO : define
    //update_overall_state();

    return ret;
}


//
void print_system_state()
{
    char print_array[ 500 ];

    add_line( "" );
    add_line( "System State:" );

    add_line( "Throttle");

    sprintf( print_array, "enabled: %d state: %d override: %d",
            system_state.throttle_module_state.control_state,
            system_state.throttle_module_state.module_state,
            system_state.throttle_module_state.override_triggered );

    add_line( print_array );

    add_line( "Steering");

    sprintf( print_array, "enabled: %d state: %d override: %d",
            system_state.steering_module_state.control_state,
            system_state.steering_module_state.module_state,
            system_state.steering_module_state.override_triggered );

    add_line( print_array );

    add_line( "Brake");

    sprintf( print_array, "enabled: %d state: %d override: %d",
            system_state.brake_module_state.control_state,
            system_state.brake_module_state.module_state,
            system_state.brake_module_state.override_triggered );

    add_line( print_array );
}
