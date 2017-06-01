/**
 * @file init.cpp
 *
 */


#include <avr/wdt.h>
#include "oscc_serial.h"
#include "oscc_can.h"
#include "debug.h"
#include "oscc_time.h"

#include "globals.h"
#include "init.h"


void init_globals( void )
{
    memset(
        &g_tx_heartbeat,
        0,
        sizeof(g_tx_heartbeat) );

    memset(
        &g_tx_chassis_state_1,
        0,
        sizeof(g_tx_chassis_state_1) );

    memset(
        &g_tx_chassis_state_2,
        0,
        sizeof(g_tx_chassis_state_2) );

    // initialize timestamps so that we don't get timeouts on start
    g_obd_steering_wheel_angle_rx_timestamp = GET_TIMESTAMP_MS();
    g_obd_wheel_speed_rx_timestamp = GET_TIMESTAMP_MS();
    g_obd_brake_pressure_rx_timestamp = GET_TIMESTAMP_MS();
    g_obd_turn_signal_rx_timestamp = GET_TIMESTAMP_MS();

    // wait a little between timestamps transmissions are offset
    g_tx_chassis_state_1.timestamp = GET_TIMESTAMP_MS();
    SLEEP_MS(5);
    g_tx_chassis_state_2.timestamp = GET_TIMESTAMP_MS();
    SLEEP_MS(5);
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial();
    #endif

    DEBUG_PRINT( "init OBD CAN - ");
    init_can( g_obd_can );

    DEBUG_PRINT( "init Control CAN - ");
    init_can( g_control_can );
}
