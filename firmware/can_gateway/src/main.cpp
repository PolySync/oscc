/**
 * @file main.cpp
 *
 */


#include <avr/wdt.h>
#include "arduino_init.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "init.h"
#include "timer.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_communication_interfaces( );

    start_timer( );

    wdt_enable( WDTO_250MS );

    DEBUG_PRINTLN( "init complete" );

    while( true )
    {
        wdt_reset();

        republish_obd_frames_to_control_can_bus( );
    }
}
