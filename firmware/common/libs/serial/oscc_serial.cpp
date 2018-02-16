/**
 * @file oscc_serial.cpp
 *
 */


#include <Arduino.h>

#include "debug.h"
#include "oscc_serial.h"


void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    DEBUG_PRINTLN( "init_serial: pass" );
}
