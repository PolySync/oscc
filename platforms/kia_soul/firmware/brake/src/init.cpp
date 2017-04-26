#include "serial.h"
#include "can.h"
#include "debug.h"
#include "time.h"

#include "globals.h"
#include "init.h"


void init_globals( void )
{
    memset( &brake_state,
            0,
            sizeof(brake_state) );

    memset( &brake_control_state,
            0,
            sizeof(brake_control_state) );

    // Initialize the timestamps to avoid timeout warnings on start up
    g_brake_command_rx_timestamp = GET_TIMESTAMP_MS( );
    g_brake_command_rx_timestamp = GET_TIMESTAMP_MS( );
}


void init_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( can );
}
