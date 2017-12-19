/**
 * @file helper.cpp
 *
 */


#include <stdlib.h>

#include "can_protocols/global_can_protocol.h"
#include "globals.h"
#include "helper.h"
#include "oscc_eeprom.h"
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

    float brake_pressure_min_in_decibars =
        oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MIN_IN_DECIBARS );

    float brake_pressure_max_in_decibars =
        oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MAX_IN_DECIBARS );

    if ( pressure < brake_pressure_min_in_decibars )
    {
        pressure = brake_pressure_min_in_decibars;
    }
    else if ( pressure > brake_pressure_max_in_decibars )
    {
        pressure = brake_pressure_max_in_decibars;
    }

    return ( pressure );
}
