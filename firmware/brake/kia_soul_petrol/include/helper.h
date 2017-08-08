/**
 * @file helper.h
 * @brief Helper functions.
 *
 */


#ifndef _OSCC_BRAKE_HELPER_H_
#define _OSCC_BRAKE_HELPER_H_


#include <stdint.h>


/*
 * @brief Scalar for converting voltage to pressure.
 *
 */
#define VOLTAGE_TO_PRESSURE_SCALAR ( 2.4 )

/*
 * @brief Offset for converting voltage to pressure.
 *
 */
#define VOLTAGE_TO_PRESSURE_OFFSET ( -252.1 )


/**
 * @brief Range of interpolation values.
 *
 * Contains input and output values for interpolation.
 *
 */
typedef struct
{
    float input_min; /* Minimum input. */

    float input_max; /* Maximum input. */

    float output_min; /* Minimum output. */

    float output_max; /* Maximum output. */
} interpolate_range_s;


// ****************************************************************************
// Function:    interpolate
//
// Purpose:     Perform a linear interpolation.
//
//              This functions specifically performs a linear interpolation of
//              form y = mx + b.
//
//              1) Normalize the input in the range from 0 to 1
//              2) Scale the output over the range defined by the output min
//                 and max values
//              3) Translate the final result into the output range
//
// Returns:     float output
//
// Parameters:  [in] input - value in the input range
//              [out] range - structure that defines the input and output ranges
//
// ****************************************************************************
float interpolate(
    const float input,
    const interpolate_range_s * const range );


// ****************************************************************************
// Function:    raw_adc_to_pressure
//
// Purpose:     Convert the raw ADC reading (0 - 1023) to a pressure between
//              1.2 - 90.0 bar.
//
//              Pressures are measured in tenths of a bar (decibars)
//              to match the values used on the vehicle; the range is
//              actually 12.0 - 900.0 decibars.
//
//              pressure = m( raw adc ) + b
//
//              Empirically determined:
//              m = 2.4
//              b = -252.1
//
//              pressure = 2.4 * ( raw adc bits ) - 252.1
//
// Returns:     float - pressure
//
// Parameters:  [in] input - raw ADC reading
//
// ****************************************************************************
float raw_adc_to_pressure(
    const int input );


#endif /* _OSCC_BRAKE_HELPER_H_ */
