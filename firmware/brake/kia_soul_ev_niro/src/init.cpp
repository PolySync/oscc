/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/brake_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "oscc_can.h"
#include "oscc_serial.h"


void init_globals( void )
{
    g_brake_control_state.enabled = false;
    g_brake_control_state.operator_override = false;
    g_brake_control_state.dtcs = 0;

    g_brake_command_timeout = false;
}


void init_devices( void )
{
    pinMode( PIN_DAC_CHIP_SELECT, OUTPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SPOOF_HIGH, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );
    pinMode( PIN_BRAKE_LIGHT_ENABLE, OUTPUT );

    cli();
    digitalWrite( PIN_DAC_CHIP_SELECT, HIGH );
    digitalWrite( PIN_SPOOF_ENABLE, LOW );
    digitalWrite( PIN_BRAKE_LIGHT_ENABLE, LOW );
    sei();
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( g_control_can );
}
