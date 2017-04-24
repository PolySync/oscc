#include <Arduino.h>

#include "globals.h"
#include "helper.h"

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


float raw_adc_to_pressure( const uint16_t input )
{
    float pressure = ( float )input;
    pressure *= 2.4;
    pressure -= 252.1;

    if ( pressure < PARAM_BRAKE_PRESSURE_MIN_IN_DECIBARS )
    {
        pressure = PARAM_BRAKE_PRESSURE_MIN_IN_DECIBARS;
    }
    else if ( pressure > PARAM_BRAKE_PRESSURE_MAX_IN_DECIBARS )
    {
        pressure = PARAM_BRAKE_PRESSURE_MAX_IN_DECIBARS;
    }

    return ( pressure );
}
