#include <Arduino.h>

#include "helper.h"
#include "brake_module.h"

float interpolate( float input, interpolate_range_s* range )
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


float raw_adc_to_pressure( uint16_t input, kia_soul_brake_module_s *brake_module )
{
    float pressure = ( float )input;
    pressure *= 2.4;
    pressure -= 252.1;

    if ( pressure < brake_module->params.min_brake_pressure )
    {
        pressure = brake_module->params.min_brake_pressure;
    }
    else if ( pressure > brake_module->params.max_brake_pressure )
    {
        pressure = brake_module->params.max_brake_pressure;
    }

    return ( pressure );
}
