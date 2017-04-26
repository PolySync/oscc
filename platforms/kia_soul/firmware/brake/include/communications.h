#ifndef _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_


// *****************************************************
// Function:    publish_brake_report
//
// Purpose:     Fill out the transmit CAN frame with the brake report
//              and publish that information on the CAN bus
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void publish_brake_report( void );


// *****************************************************
// Function:    publish_timed_tx_frames
//
// Purpose:     Determine if enough time has passed to publish the braking report
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void publish_timed_tx_frames( void );


// *****************************************************
// Function:    process_brake_command
//
// Purpose:     Process a brake command message
//
// Returns:     void
//
// Parameters:  data -  pointer to a brake command control message
//
// *****************************************************
void process_brake_command(
    const uint8_t * data );


// *****************************************************
// Function:    process_chassis_state_1
//
// Purpose:     Process the chassis state message
//
// Returns:     void
//
// Parameters:  data - pointer to a chassis state message that contains
//                             the brake pressure
//
// *****************************************************
void process_chassis_state_1(
    const uint8_t * data );


// *****************************************************
// Function:    handle_ready_rx_frames
//
// Purpose:     Parse received CAN data and redirect to correct
//              processing function
//
// Returns:     void
//
// Parameters:  rx_frame - pointer to struct containing to CAN frame
//
// *****************************************************
void handle_ready_rx_frames(
    const can_frame_s * const rx_frame );


// *****************************************************
// Function:    check_rx_timeouts
//
// Purpose:     If the control is currently enabled, but the receiver indicates
//              a "watchdog" timeout, then disable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void check_rx_timeouts( void );


#endif
