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


int main( void )
{
    init_arduino( );

    init_globals( );

    init_communication_interfaces( );

    SET_HEARTBEAT_STATE( OSCC_REPORT_HEARTBEAT_STATE_OK );

    wdt_enable( WDTO_120MS );

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        wdt_reset();

        check_for_incoming_message( );

        check_for_controller_command_timeout( );

        publish_reports( );
    }
}
