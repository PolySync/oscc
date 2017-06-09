/**
 * @file main.cpp
 *
 */


#include <avr/wdt.h>
#include "arduino_init.h"
#include "debug.h"
#include "oscc_time.h"

#include "init.h"
#include "globals.h"
#include "communications.h"
#include "steering_control.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    init_devices( );

    init_communication_interfaces( );

    wdt_enable( WDTO_120MS );

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        wdt_reset();

        check_for_incoming_message( );

        check_for_timeouts( );

        check_for_sensor_faults( );

        uint32_t time_since_last_control_loop_in_msec = get_time_delta(
            g_last_control_loop_timestamp,
            GET_TIMESTAMP_MS());

        if( time_since_last_control_loop_in_msec > CONTROL_LOOP_INTERVAL_IN_MSEC )
        {
            check_for_operator_override( );

            update_steering( );

            g_last_control_loop_timestamp = GET_TIMESTAMP_MS();
        }

        publish_reports( );
    }
}
