/**
 * @file init.cpp
 *
 */


#include <Arduino.h>
#include "oscc_serial.h"
#include "oscc_can.h"
#include "debug.h"
#include "oscc_time.h"
#include "oscc_pid.h"

#include "init.h"
#include "globals.h"


void init_globals( void )
{
    memset( &g_steering_control_state,
            0,
            sizeof(g_steering_control_state) );

    // Initialize the timestamps to avoid timeout warnings on start up
    g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
    g_chassis_state_1_report_last_rx_timestamp = GET_TIMESTAMP_MS( );
    g_last_control_loop_timestamp = 0;

    pid_zeroize( &g_pid, PID_WINDUP_GUARD );
    g_pid.proportional_gain = PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain = PID_INTEGRAL_GAIN;
    g_pid.derivative_gain = PID_DERIVATIVE_GAIN;
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
    init_can( g_control_can );
}
