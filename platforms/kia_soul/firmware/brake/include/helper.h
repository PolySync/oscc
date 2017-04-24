#ifndef _OSCC_KIA_SOUL_BRAKE_HELPER_H_
#define _OSCC_KIA_SOUL_BRAKE_HELPER_H_


#include <stdint.h>


typedef struct
{
    float input_min;
    float input_max;
    float output_min;
    float output_max;
} interpolate_range_s;


// *****************************************************
// Function:    interpolate
//
// Purpose:     Perform a linear interpolation
//              This functions specifically performs a linear interpolation of
//              form y = mx + b.
//              1) Normalize the input in the range from 0 to 1
//              2) Scale the output over the range defined by the output min
//                 and max values
//              3) Translate the final result into the output range
//
// Returns:     float output
//
// Parameters:  input - value in the input range
//              range - structure that defines the input and output ranges
//
// *****************************************************
float interpolate( float input, interpolate_range_s* range );


// *****************************************************
// Function:    raw_adc_to_pressure
//
// Purpose:     Convert the raw ADC reading (0 - 1023)
//              to a pressure between 1.2 - 90.0 bar
//
//              Pressures are measured in tenths of a bar (decibars)
//              to match the values used on the vehicle; the range is
//              actually 12.0 - 900.0 decibars
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
// Parameters:  input - raw ADC reading
//
// *****************************************************
float raw_adc_to_pressure( uint16_t input );


#endif
