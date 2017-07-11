/**
 * @file communications.h
 * @brief Communication functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_


#include <stdint.h>
#include "oscc_can.h"


/*
 * @brief Amount of time after controller command that is considered a
 *        timeout. [milliseconds]
 *
 */
#define COMMAND_TIMEOUT_IN_MSEC ( 650 )


/*
 * @brief Amount of time after an OBD frame is received that is considered a
 *        timeout. [milliseconds]
 *
 */
#define OBD_TIMEOUT_IN_MSEC ( 500 )


// ****************************************************************************
// Function:    publish_brake_report
//
// Purpose:     Publish brake report to CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void publish_brake_report( void );


// ****************************************************************************
// Function:    publish_fault_report
//
// Purpose:     Publish a fault report message to the CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void publish_fault_report( void );


// ****************************************************************************
// Function:    check_for_timeouts
//
// Purpose:     Check for command and report timeouts.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_timeouts( void );


// ****************************************************************************
// Function:    check_for_can_frame
//
// Purpose:     Check CAN bus for incoming messages and process any present.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_can_frame( void );


#endif /* _OSCC_KIA_SOUL_BRAKE_COMMUNICATIONS_H_ */
