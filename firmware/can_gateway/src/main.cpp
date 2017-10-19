/**
 * @file main.cpp
 *
 */


#include <avr/wdt.h>

#include "arduino_init.h"
#include "communications.h"
#include "debug.h"
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

        check_for_module_reports( );

        republish_obd_frames_to_control_can_bus( );
    }
}
