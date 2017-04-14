#include <HardwareSerial.h>
#include "serial.h"
#include "debug.h"

void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    DEBUG_PRINTLN( "init_serial: pass" );
}