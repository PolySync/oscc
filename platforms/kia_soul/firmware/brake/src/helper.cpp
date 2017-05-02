/**
 * @file helper.cpp
 *
 */


#include <stdint.h>

#include "globals.h"
#include "helper.h"


/*
 * @brief Scalar for converting voltage to pressure.
 *
 */
#define VOLTAGE_TO_PRESSURE_SCALAR (2.4)

/*
 * @brief Offset for converting voltage to pressure.
 *
 */
#define VOLTAGE_TO_PRESSURE_OFFSET (-252.1)


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
    const uint16_t input )
{
    float pressure = ( float )input;
    pressure *= VOLTAGE_TO_PRESSURE_SCALAR;
    pressure += VOLTAGE_TO_PRESSURE_OFFSET;

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
