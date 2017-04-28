/**
 * @file serial.h
 *
 */


#include <Arduino.h>
#include "debug.h"
#include "serial.h"


void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    DEBUG_PRINTLN( "init_serial: pass" );
}
