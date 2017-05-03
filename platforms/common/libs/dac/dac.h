/**
 * @file helper.h
 * @brief Helper functions.
 *
 */


#ifndef _OSCC_DAC_H_
#define _OSCC_DAC_H_


#include "DAC_MCP49xx.h"

/*
 * @brief Number of bits to shift to go from a 10-bit value to a 12-bit value.
 *
 */
#define BIT_SHIFT_10BIT_TO_12BIT ( 2 )


// ****************************************************************************
// Function:    write_sample_averages_to_dac
//
// Purpose:     Samples two analog pins and writes their averages to a DAC.
//
// Returns:     void
//
// Parameters:  [in] dac - Reference to DAC_MCP49xx object.
//              [in] num_samples - Number of samples to take.
//              [in] signal_pin_1 - First signal pin to sample.
//              [in] signal_pin_2 - Second signal pin to sample.
//
// ****************************************************************************
void write_sample_averages_to_dac(
        DAC_MCP49xx & dac,
        const int16_t num_samples,
        const uint8_t signal_pin_1,
        const uint8_t signal_pin_2 );



#endif /* _OSCC_CAN_H_ */
