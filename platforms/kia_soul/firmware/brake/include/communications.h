#ifndef _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_


#include <stdint.h>
#include "can.h"


// *****************************************************
// Function:    publish_reports
//
// Purpose:     Determine if enough time has passed to publish the braking report
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void publish_reports( void );


// *****************************************************
// Function:    check_for_controller_command_timeout
//
// Purpose:     If the control is currently enabled, but the receiver indicates
//              a "watchdog" timeout, then disable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void check_for_controller_command_timeout( void );


// *****************************************************
// Function:    check_for_incoming_message
//
// Purpose:     Check for incoming CAN frame.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void check_for_incoming_message( void );


#endif
