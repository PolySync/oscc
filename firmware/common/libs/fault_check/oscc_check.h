/**
 * @file oscc_check.h
 * @brief common fault checking functions.
 *
 */


#ifndef _OSCC_CHECK_H_
#define _OSCC_CHECK_H_

#include <stdint.h>

// ****************************************************************************
// Function:    check_voltage_grounded
//
// Purpose:     Check if the voltage is ground for the period of time determined
//              by the configuration define of HYSTERESIS. This function is
//              assumed to be called periodicly.
//
// Returns:     bool where true means there is a fault after the HYSTERESIS time
//
// Parameters:  [in] high - the high pin to check for ground
//              [in] low - the low pin to check for ground
//
// ****************************************************************************
bool check_voltage_grounded( uint16_t high, uint16_t low );

#endif /* _OSCC_CHECK_H_ */
