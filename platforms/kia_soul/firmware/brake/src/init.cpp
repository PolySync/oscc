#include <Arduino.h>
#include "serial.h"
#include "can.h"
#include "debug.h"
#include "time.h"

#include "globals.h"
#include "init.h"
#include "accumulator.h"
#include "master_cylinder.h"
#include "brake_control.h"


void init_globals( void )
{
    memset( &brake_state,
            0,
            sizeof(brake_state) );

    memset( &brake_control_state,
            0,
            sizeof(brake_control_state) );

    // Initialize the timestamps to avoid timeout warnings on start up
    g_brake_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    g_brake_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
}


void init_devices( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    accumulator_init( );
    master_cylinder_init( );
    brake_init( );
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( can );
}
