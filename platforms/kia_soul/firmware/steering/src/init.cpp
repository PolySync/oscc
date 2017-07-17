/**
 * @file init.cpp
 *
 */


#include <Arduino.h>
#include "oscc_serial.h"
#include "oscc_can.h"
#include "steering_can_protocol.h"
#include "debug.h"

#include "globals.h"
#include "communications.h"
#include "init.h"


void init_globals( void )
{
    memset( &g_steering_control_state,
            0,
            sizeof(g_steering_control_state) );

    g_steering_command_timeout = false;
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
