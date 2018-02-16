/**
 * @file communications.h
 * @brief Communication functionality.
 *
 */


#ifndef _OSCC_STEERING_COMMUNICATIONS_H_
#define _OSCC_STEERING_COMMUNICATIONS_H_


// ****************************************************************************
// Function:    publish_steering_report
//
// Purpose:     Publish steering report to CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void publish_steering_report( void );


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
// Function:    check_for_incoming_message
//
// Purpose:     Check CAN bus for incoming frames and process any present.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_incoming_message( void );


#endif /* _OSCC_STEERING_COMMUNICATIONS_H_ */
