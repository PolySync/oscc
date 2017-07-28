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
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
// #include "chassis_state_can_protocol.h"
// #include "gateway_can_protocol.h"




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
            get_can_msg_array_index_reference( OSCC_STEERING_COMMAND_CAN_ID );

    const can_frame_s * const steering_report_frame =
            get_can_msg_array_index_reference( OSCC_STEERING_REPORT_CAN_ID );

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
            get_can_msg_array_index_reference( OSCC_THROTTLE_COMMAND_CAN_ID );

    const can_frame_s * const throttle_report_frame =
            get_can_msg_array_index_reference( OSCC_THROTTLE_REPORT_CAN_ID );

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
            get_can_msg_array_index_reference( OSCC_BRAKE_COMMAND_CAN_ID );

    const can_frame_s * const brake_report_frame =
            get_can_msg_array_index_reference( OSCC_BRAKE_REPORT_CAN_ID );

    if( brake_command_frame != NULL  ||  brake_report_frame != NULL )
    {
        ret = analyze_brake_state(
                &system_state.brake_module_state,
                brake_command_frame,
                brake_report_frame );
    }

    return ret;
}


// //
// static int update_gateway_state()
// {
//     int ret = ERROR;

//     const can_frame_s * const heartbeat_msg_frame =
//             get_can_msg_array_index_reference( OSCC_REPORT_HEARTBEAT_CAN_ID );

//     const can_frame_s * const chassis_state1_frame =
//             get_can_msg_array_index_reference( OSCC_REPORT_CHASSIS_STATE_1_CAN_ID );

//     const can_frame_s * const chassis_state2_frame =
//             get_can_msg_array_index_reference( OSCC_REPORT_CHASSIS_STATE_2_CAN_ID );

//     if( heartbeat_msg_frame != NULL ||
//             chassis_state1_frame != NULL ||
//             chassis_state2_frame != NULL )
//     {
//         ret = analyze_gateway_state(
//                 &system_state.gateway_module_state,
//                 heartbeat_msg_frame,
//                 chassis_state1_frame,
//                 chassis_state2_frame );
//     }

//     return ret;
// }




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

//     if( update_gateway_state() == ERROR )
//     {
//         system_state.gateway_module_state.module_state = STATE_FAULT;

//         ret = ERROR;
//     }

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
