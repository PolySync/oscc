/**
 * @file helper.cpp
 *
 */


#include <stdlib.h>

#include "globals.h"
#include "helper.h"
#include "vehicles.h"


float interpolate(
    const float input,
    const interpolate_range_s * const range )
{
    float output = input;

    if ( range != NULL )
    {
        output = ( input - range->input_min );
        output /= ( range->input_max - range->input_min );
        output *= ( range->output_max - range->output_min );
        output += range->output_min;
    }

    return ( output );
}


float raw_adc_to_pressure(
    const int input )
{
    float pressure = ( float )input;
    pressure *= VOLTAGE_TO_PRESSURE_SCALAR;
    pressure += VOLTAGE_TO_PRESSURE_OFFSET;

    if ( pressure < BRAKE_PRESSURE_MIN_IN_DECIBARS )
    {
        pressure = BRAKE_PRESSURE_MIN_IN_DECIBARS;
    }
    else if ( pressure > BRAKE_PRESSURE_MAX_IN_DECIBARS )
    {
        pressure = BRAKE_PRESSURE_MAX_IN_DECIBARS;
    }

    return ( pressure );
}
