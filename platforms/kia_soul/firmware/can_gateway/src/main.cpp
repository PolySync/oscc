/**
 * @file can_gateway.ino
 * @brief CAN Gateway Module Source.
 *
 * Board: Arduino Uno
 * Arduino Build/Version: 1.6.7 linux-x86_64
 *
 * @warning Requires watchdog reset support in the bootloader, which is NOT supported
 * in all Arduino bootloaders.
 *
 */


#include <avr/wdt.h>

#include "arduino_init.h"
#include "debug.h"

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
