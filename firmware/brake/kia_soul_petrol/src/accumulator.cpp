/**
 * @file accumulator.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/global_can_protocol.h"
#include "accumulator.h"
#include "debug.h"
#include "globals.h"
#include "helper.h"
#include "oscc_eeprom.h"
#include "vehicles.h"


void accumulator_init( void )
{
    pinMode( PIN_ACCUMULATOR_PUMP_MOTOR, OUTPUT );

    accumulator_turn_pump_off( );
}


void accumulator_turn_pump_off( void )
{
    cli();
    digitalWrite( PIN_ACCUMULATOR_PUMP_MOTOR, LOW );
    sei();
}


void accumulator_turn_pump_on( void )
{
    cli();
    digitalWrite( PIN_ACCUMULATOR_PUMP_MOTOR, HIGH );
    sei();
}


float accumulator_read_pressure( void )
{
    cli();
    int raw_adc = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );
    sei();

    float pressure = raw_adc_to_pressure( raw_adc );

    return pressure;
}


void accumulator_maintain_pressure( void )
{
    float pressure = accumulator_read_pressure( );

    if ( pressure <= g_eeprom_config.accumulator_pressure_min_in_decibars )
    {
        accumulator_turn_pump_on( );
    }
    else if ( pressure >= g_eeprom_config.accumulator_pressure_max_in_decibars )
    {
        accumulator_turn_pump_off( );
    }
}
