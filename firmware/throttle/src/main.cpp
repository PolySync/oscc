/**
 * @file main.cpp
 *
 */


#include "arduino_init.h"
#include "communications.h"
#include "debug.h"
#include "init.h"
#include "status.h"
#include "timers.h"
#include "throttle_control.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_status( );

    init_devices( );

    init_communication_interfaces( );

    start_timers( );

    DEBUG_PRINTLN( "init complete" );
    status_ledsOff( );

    while( true )
    {
        check_for_incoming_message( );

        check_for_operator_override( );
    }
}
