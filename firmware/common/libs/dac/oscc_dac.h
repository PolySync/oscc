/**
 * @file oscc_dac.h
 * @brief OSCC DAC interface.
 *
 */


#ifndef _OSCC_DAC_H_
#define _OSCC_DAC_H_


#include "DAC_MCP49xx.h"


/*
 * @brief Number of bits to shift to go from a 10-bit value to a 12-bit value.
 *
 */
#define DAC_BIT_SHIFT_10BIT_TO_12BIT ( 2 )


// ****************************************************************************
// Function:    prevent_signal_discontinuity
//
// Purpose:     Samples the current values, smooths them, and writes the measured
//              sensor values to the DAC to avoid a signal discontinuity when
//              control changes from module to vehicle or vehicle to module. If
//              a smoothing doesn't occur then there is the possibility of the
//              vehicle going into a fault state when it detects an abrupt change.
//
// Returns:     void
//
// Parameters:  [in] dac - Reference to DAC_MCP49xx object.
//              [in] num_samples - Number of samples to take.
//              [in] signal_pin_1 - First signal pin to sample.
//              [in] signal_pin_2 - Second signal pin to sample.
//
// ****************************************************************************
void prevent_signal_discontinuity(
        DAC_MCP49xx & dac,
        const int16_t num_samples,
        const uint8_t signal_pin_1,
        const uint8_t signal_pin_2 );



#endif /* _OSCC_CAN_H_ */
