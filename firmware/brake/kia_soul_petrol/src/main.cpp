/**
 * @file main.cpp
 *
 */


#include "arduino_init.h"

#include "debug.h"
#include "accumulator.h"
#include "brake_control.h"
#include "communications.h"
#include "timers.h"
#include "init.h"


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
