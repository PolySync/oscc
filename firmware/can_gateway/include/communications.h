/**
 * @file communications.h
 * @brief Communication functionality.
 *
 */


#ifndef _OSCC_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_CAN_GATEWAY_COMMUNICATIONS_H_


#include "globals.h"


// ****************************************************************************
// Function:    republish_obd_frames_to_control_can_bus
//
// Purpose:     Republish pertinent frames on the OBD CAN bus to the Control CAN
//              bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void republish_obd_frames_to_control_can_bus( void );


#endif /* _OSCC_CAN_GATEWAY_COMMUNICATIONS_H_ */
