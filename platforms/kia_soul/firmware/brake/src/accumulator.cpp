#include <Arduino.h>
#include <stdint.h>

#include "accumulator.h"
#include "helper.h"


void accumulator_init( kia_soul_brake_module_s *brake_module )
{
    pinMode( brake_module->pins.accumulator_pump, OUTPUT );

    accumulator_turn_pump_off( brake_module );
}


void accumulator_turn_pump_off( kia_soul_brake_module_s *brake_module )
{
    digitalWrite( brake_module->pins.accumulator_pump, LOW );
}


void accumulator_turn_pump_on( kia_soul_brake_module_s *brake_module )
{
    digitalWrite( brake_module->pins.accumulator_pump, HIGH );
}


void accumulator_maintain_pressure( kia_soul_brake_module_s *brake_module )
{
    const float accumulator_alpha = 0.05;

    uint16_t raw_accumulator_data = analogRead( brake_module->pins.pacc );

    float pressure = raw_adc_to_pressure( raw_accumulator_data, brake_module );

    brake_module->state.accumulator.pressure =
        ( accumulator_alpha * pressure ) +
        ( ( 1.0 - accumulator_alpha ) * brake_module->state.accumulator.pressure );

    if ( brake_module->state.accumulator.pressure < brake_module->params.min_accumulator_pressure )
    {
        accumulator_turn_pump_on( brake_module );
    }

    if ( brake_module->state.accumulator.pressure > brake_module->params.max_accumulator_pressure )
    {
        accumulator_turn_pump_off( brake_module );
    }
}
