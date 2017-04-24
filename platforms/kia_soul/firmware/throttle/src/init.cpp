#include <Arduino.h>
#include "serial.h"
#include "can.h"

#include "globals.h"
#include "init.h"


void init_structs_to_zero( void )
{
    memset( &rx_frame_throttle_command,
            0,
            sizeof( rx_frame_throttle_command ) );
}


void init_pins( void )
{
    pinMode( PIN_DAC_CS, OUTPUT );
    pinMode( PIN_ACCEL_POS_SENSOR_HIGH, INPUT );
    pinMode( PIN_ACCEL_POS_SENSOR_LOW, INPUT );
    pinMode( PIN_ACCEL_POS_SPOOF_HIGH, INPUT );
    pinMode( PIN_ACCEL_POS_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    digitalWrite( PIN_DAC_CS, HIGH ); // Deselect DAC CS

    digitalWrite( PIN_SPOOF_ENABLE, LOW );
}


void init_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    init_can( can );
}
