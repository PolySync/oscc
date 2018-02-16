/**
* @file gateway_module_state.c
* @brief gateway module Source.
*
*/




#include <stdlib.h>
#include <stdio.h>
#include "can_protocols/gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"

#include "macros.h"
#include "can_monitor.h"
#include "system_state.h"
#include "gateway_module_state.h"




// *****************************************************
// static definitions
// *****************************************************


//
static int analyze_heartbeat_msg_frame(
        gateway_module_state_s * const state,
        const oscc_report_heartbeat_data_s * const heartbeat_msg )
{
    int module_state = STATE_OK;

    return module_state;
}


//
static int analyze_chassis_state1_frame(
        gateway_module_state_s * const state,
        const oscc_report_chassis_state_1_data_s * const chassis_state1 )
{
    int module_state = STATE_OK;

    // TODO: do steering_wheel_angle, steering_wheel_angle_rate, brake_pressure checks

    return module_state;
}


//
static int analyze_chassis_state2_frame(
        gateway_module_state_s * const state,
        const oscc_report_chassis_state_2_data_s * const chassis_state2 )
{
    int module_state = STATE_OK;

    // TODO: do wheel speed range check

    return module_state;
}




// *****************************************************
// public definitions
// *****************************************************


//
int analyze_gateway_state(
        gateway_module_state_s * const state,
        const can_frame_s * const heartbeat_msg_frame,
        const can_frame_s * const chassis_state1_frame,
        const can_frame_s * const chassis_state2_frame )
{
    int ret = NOERR;

    analyze_heartbeat_msg_frame(
            state,
            (oscc_report_heartbeat_data_s*)
                    heartbeat_msg_frame->frame_contents.buffer );

    analyze_chassis_state1_frame(
            state,
            (oscc_report_chassis_state_1_data_s*)
                    chassis_state1_frame->frame_contents.buffer );

    analyze_chassis_state2_frame(
            state,
            (oscc_report_chassis_state_2_data_s*)
                    chassis_state2_frame->frame_contents.buffer );

    return ret;
}
