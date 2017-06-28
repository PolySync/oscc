/**
 * @file main.cpp
 *
 */


#include <avr/wdt.h>

#include "arduino_init.h"
#include "debug.h"
#include "gateway_can_protocol.h"

#include "globals.h"
#include "communications.h"
#include "init.h"
#include "display.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_communication_interfaces( );

    SET_HEARTBEAT_STATE( OSCC_REPORT_HEARTBEAT_STATE_OK );

    wdt_enable( WDTO_250MS );

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        wdt_reset();

        check_for_incoming_message( );

        update_display( );

        check_for_obd_timeout( );

        publish_reports( );
    }
}
