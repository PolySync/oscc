#ifndef _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_

#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"

#include "steering_module.h"



// *****************************************************
// Function:    publish_ps_ctrl_steering_report
//
// Purpose:     Fill out the transmit CAN frame with the steering angle
//              and publish that information on the CAN bus
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void publish_ps_ctrl_steering_report(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *report,
    MCP_CAN &can,
    uint8_t torque_sum );


// *****************************************************
// Function:    publish_timed_tx_frames
//
// Purpose:     Determine if enough time has passed to publish the steering
//              report to the CAN bus again
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void publish_timed_tx_frames(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *report,
    MCP_CAN &can,
    uint8_t torque_sum );


// *****************************************************
// Function:    process_ps_ctrl_steering_command
//
// Purpose:     Process a steering command message
//
// Returns:     void
//
// Parameters:  control_data -  pointer to a steering command control message
//
// *****************************************************
void process_ps_ctrl_steering_command(
    kia_soul_steering_module_s *steering_module,
    const ps_ctrl_steering_command_msg * const control_data,
    can_frame_s *command,
    DAC_MCP49xx &dac );


// *****************************************************
// Function:    process_psvc_chassis_state1
//
// Purpose:     Process the chassis state message
//
// Returns:     void
//
// Parameters:  chassis_data - pointer to a chassis state message that contains
//                             the steering angle
//
// *****************************************************
void process_psvc_chassis_state1(
    kia_soul_steering_module_s *steering_module,
    const psvc_chassis_state1_data_s * const chassis_data );


// *****************************************************
// Function:    handle_ready_rx_frames
//
// Purpose:     Parse received CAN data and redirect to correct
//              processing function
//
// Returns:     void
//
// Parameters:  frame - frame containing received data
//
// *****************************************************
void handle_ready_rx_frames(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *frame,
    can_frame_s *command,
    DAC_MCP49xx &dac );


// *****************************************************
// Function:    check_rx_timeouts
//
// Purpose:     If the control is currently enabled, but the receiver indicates
//              a "watchdog" timeout, then disable the control
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void check_rx_timeouts(
    kia_soul_steering_module_s *steering_module,
    can_frame_s *command,
    DAC_MCP49xx &dac );


#endif
