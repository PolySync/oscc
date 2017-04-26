#ifndef _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_


#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"


// *****************************************************
// Function:    publish_steering_report
//
// Purpose:     Fill out the transmit CAN frame with the steering angle
//              and publish that information on the CAN bus
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void publish_steering_report( void );


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
void publish_timed_tx_frames( void );


// *****************************************************
// Function:    process_steering_command
//
// Purpose:     Process a steering command message
//
// Returns:     void
//
// Parameters:  control_data -  pointer to a steering command control message
//
// *****************************************************
void process_steering_command( void );


// *****************************************************
// Function:    process_chassis_state_1
//
// Purpose:     Process the chassis state message
//
// Returns:     void
//
// Parameters:  chassis_data - pointer to a chassis state message that contains
//                             the steering angle
//
// *****************************************************
void process_chassis_state_1(
    const oscc_report_chassis_state_1_data_s * const chassis_data );


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
void handle_ready_rx_frame( can_frame_s * const frame );


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
void check_rx_timeouts( void );


#endif
