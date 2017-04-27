#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_


#include <stdint.h>

#include "globals.h"


#define SET_HEARTBEAT_WARNING(x) (tx_heartbeat.data.warning_register |= ((uint16_t) x))
#define CLEAR_HEARTBEAT_WARNING(x) (tx_heartbeat.data.warning_register &= ~((uint16_t) x))
#define SET_HEARTBEAT_ERROR(x) (tx_heartbeat.data.error_register |= ((uint16_t) x))
#define CLEAR_HEARTBEAT_ERROR(x) (tx_heartbeat.data.error_register &= ~((uint16_t) x))
#define SET_HEARTBEAT_STATE(x) (tx_heartbeat.data.state = ((uint8_t) x))
#define GET_HEARTBEAT_STATE() (tx_heartbeat.data.state)

#define SET_CHASSIS_1_FLAG(x) (tx_chassis_state_1.data.flags |= ((uint8_t) x))
#define CLEAR_CHASSIS_1_FLAG(x) (tx_chassis_state_1.data.flags &= ~((uint8_t) x))


// *****************************************************
// Function:    publish_heartbeat_report
//
// Purpose:     Fill out heartbeat report and publish it.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void publish_heartbeat_report( void );


// *****************************************************
// Function:    publish_reports
//
// Purpose:     Determine if enough time has passed to publish reports.
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
