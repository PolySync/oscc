/**
 * @file main.cpp
 *
 */


#include "arduino_init.h"

#include "debug.h"
#include "accumulator.h"
#include "brake_control.h"
#include "communications.h"
#include "init.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_devices( );

    init_communication_interfaces( );

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        check_for_can_frame( );

        accumulator_maintain_pressure( );

        check_for_timeouts( );

        check_for_sensor_faults( );

        check_for_operator_override( );

        update_brake( );
    }
}
