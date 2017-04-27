#include <avr/wdt.h>
#include "serial.h"
#include "can.h"
#include "debug.h"
#include "time.h"

#include "globals.h"
#include "init.h"


void init_globals( void )
{
    memset(
        &tx_heartbeat,
        0,
        sizeof(tx_heartbeat) );

    memset(
        &tx_chassis_state_1,
        0,
        sizeof(tx_chassis_state_1) );

    memset(
        &tx_chassis_state_2,
        0,
        sizeof(tx_chassis_state_2) );

    // initialize timestamps so that we don't get timeouts on start
    obd_steering_wheel_angle_rx_timestamp = GET_TIMESTAMP_MS();
    obd_wheel_speed_rx_timestamp = GET_TIMESTAMP_MS();
    obd_brake_pressure_rx_timestamp = GET_TIMESTAMP_MS();
    obd_turn_signal_rx_timestamp = GET_TIMESTAMP_MS();

    // wait a little between timestamps transmissions are offset
    tx_chassis_state_1.timestamp = GET_TIMESTAMP_MS();
    SLEEP_MS(5);
    tx_chassis_state_2.timestamp = GET_TIMESTAMP_MS();
    SLEEP_MS(5);
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial();
    #endif

    DEBUG_PRINT( "init OBD CAN - ");
    init_can( obd_can );

    DEBUG_PRINT( "init Control CAN - ");
    init_can( control_can );
}
