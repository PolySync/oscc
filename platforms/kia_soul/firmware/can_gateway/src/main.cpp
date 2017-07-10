/**
 * @file main.cpp
 *
 */


#include <avr/wdt.h>
#include "arduino_init.h"
#include "debug.h"

#include "init.h"
#include "communications.h"


int main( void )
{
    init_arduino( );

    init_communication_interfaces( );

    wdt_enable( WDTO_120MS );

    DEBUG_PRINTLN( "init complete" );

    while( true )
    {
        wdt_reset();

        republish_obd_frames_to_control_can_bus( );
    }
}
