#include <Arduino.h>
#include "serial.h"
#include "can.h"
#include "debug.h"
#include "time.h"
#include "pid.h"

#include "init.h"
#include "globals.h"


void init_globals( void )
{
    memset( &steering_control_state,
            0,
            sizeof(steering_control_state) );

    // Initialize the timestamps to avoid timeout warnings on start up
    g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );

    pid_zeroize( &pid, PARAM_PID_WINDUP_GUARD );
}


void init_devices( void )
{
    pinMode( PIN_DAC_CHIP_SELECT, OUTPUT );
    pinMode( PIN_TORQUE_SENSOR_HIGH, INPUT );
    pinMode( PIN_TORQUE_SENSOR_LOW, INPUT );
    pinMode( PIN_TORQUE_SPOOF_HIGH, INPUT );
    pinMode( PIN_TORQUE_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    digitalWrite( PIN_DAC_CHIP_SELECT, HIGH );

    digitalWrite( PIN_SPOOF_ENABLE, LOW );
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( control_can );
}
