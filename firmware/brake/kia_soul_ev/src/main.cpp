/**
 * @file main.cpp
 *
 */


#include "arduino_init.h"
#include "brake_control.h"
#include "communications.h"
#include "debug.h"
#include "init.h"
#include "timers.h"
#include "oscc_eeprom.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_devices( );

    init_communication_interfaces( );

    #ifdef RESET_CONFIG
    init_config( );
    #endif

    start_timers( );

    DEBUG_PRINTLN( "init complete" );

    while( true )
    {
        check_for_incoming_message( );

        check_for_operator_override( );
    }
}
