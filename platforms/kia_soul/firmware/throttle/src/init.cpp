#include <Arduino.h>
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "init.h"


void init_globals( void )
{
    memset( &throttle_state,
            0,
            sizeof(throttle_state) );

    memset( &throttle_control_state,
            0,
            sizeof(throttle_control_state) );

    // update timestamps so we don't set timeout warnings on start up
    g_throttle_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    g_throttle_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
}


void init_pins( void )
{
    pinMode( PIN_DAC_CHIP_SELECT, OUTPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SENSOR_HIGH, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SENSOR_LOW, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SPOOF_HIGH, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    digitalWrite( PIN_DAC_CHIP_SELECT, HIGH ); // Deselect DAC CS

    digitalWrite( PIN_SPOOF_ENABLE, LOW );
}


void init_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( control_can );
}
