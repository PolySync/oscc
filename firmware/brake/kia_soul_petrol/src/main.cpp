/**
 * @file main.cpp
 *
 */


#include "accumulator.h"
#include "arduino_init.h"
#include "brake_control.h"
#include "communications.h"
#include "debug.h"
#include "init.h"
#include "timers.h"


int main( void )
{
    init_arduino( );

    init_communication_interfaces( );

    init_globals( );

    init_devices( );

    init_communication_interfaces( );

    start_timers( );

    DEBUG_PRINTLN( "init complete" );

    while( true )
    {
        check_for_incoming_message( );

        accumulator_maintain_pressure( );

        check_for_operator_override( );

        update_brake( );
    }
}
