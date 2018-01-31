/**
 * @file oscc_check.h
 * @brief common fault checking functions.
 *
 */


#ifndef _OSCC_CHECK_H_
#define _OSCC_CHECK_H_

#include <stdint.h>

typedef struct {
    bool fault_active;
    unsigned long fault_start_time;
} fault_state_s;


// ****************************************************************************
// Function:    fault_exceeded_duration
//
// Purpose:     Check if a fault has been active for longer than
//              `max_fault_duration`.
//
// Returns:     bool where true means a fault has been active for longer than
//              `max_fault_duration`.
//
// Parameters:  [in] fault_active - true if a fault has been detected.
//              [in] max_fault_duration - the maximum time that the fault can be
//                                        active without being treated as an
//                                        error.
//              [in] fault_state - whether the fault is active and at what time
//                                 the fault was first detected.
//
// ****************************************************************************
bool fault_exceeded_duration(
        bool fault_active,
        unsigned long max_fault_duration,
        fault_state_s *state);


// ****************************************************************************
// Function:    check_voltage_grounded
//
// Purpose:     Check if the voltage is grounded for the period of time
//              determined by `max_fault_duration`. This function is assumed to
//              be called periodicly.
//
// Returns:     bool where true means a fault has been active for longer than
//              `max_fault_duration`.
//
// Parameters:  [in] high - the high pin to check for ground
//              [in] low - the low pin to check for ground
//              [in] max_fault_duration - the maximum time that the fault can be
//                                        active
//              [in] fault_state - whether the fault is active and at what time
//                                 the fault was first detected
//
// ****************************************************************************
bool check_voltage_grounded(
        uint16_t high,
        uint16_t low,
        unsigned long max_fault_duration,
        fault_state_s *fault_state );

#endif /* _OSCC_CHECK_H_ */
