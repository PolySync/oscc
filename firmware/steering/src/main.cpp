/**
 * @file main.cpp
 *
 */


#include "arduino_init.h"
#include "communications.h"
#include "debug.h"
#include "init.h"
#include "status.h"
#include "steering_control.h"
#include "timers.h"


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

#ifdef STEERING_OVERRIDE
        check_for_operator_override( );
#endif
    }
}
