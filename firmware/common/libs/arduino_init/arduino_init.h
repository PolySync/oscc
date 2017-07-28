/**
 * @file arduino_init.h
 * @brief Arduino initialization.
 *
 */


#ifndef _OSCC_ARDUINO_INIT_H_
#define _OSCC_ARDUINO_INIT_H_


// ****************************************************************************
// Function:    init_arduino
//
// Purpose:     Perform initialization normally done by the compiler during
//              sketch compilation so that the firmware code can be a standard
//              main function with an infinite loop rather than an
//              Arduino-specific setup/loop system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_arduino( void );


#endif /* _OSCC_ARDUINO_INIT_H_ */
