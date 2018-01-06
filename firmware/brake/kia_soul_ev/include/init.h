/**
 * @file init.h
 * @brief Initialization functionality.
 *
 */


#ifndef _OSCC_BRAKE_INIT_H_
#define _OSCC_BRAKE_INIT_H_


// ****************************************************************************
// Function:    init_globals
//
// Purpose:     Initialize values of global variables.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_globals( void );


// ****************************************************************************
// Function:    init_devices
//
// Purpose:     Initialize physical devices on or connected to the module.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_devices( void );


// ****************************************************************************
// Function:    init_communication_interfaces
//
// Purpose:     Initialize the communication interfaces used by the module.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_communication_interfaces( void );


// ****************************************************************************
// Function:    start_timers
//
// Purpose:     Start timers for report publishing and fault checking.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void start_timers( void );


#endif /* _OSCC_BRAKE_INIT_H_ */
