#include <avr/wdt.h>
#include <Arduino.h>
#include "serial.h"
#include "can.h"

#include "globals.h"
#include "init.h"


void init_structs_to_zero( void )
{
    memset( &tx_frame_heartbeat, 0, sizeof(tx_frame_heartbeat) );
    memset( &tx_frame_chassis_state1, 0, sizeof(tx_frame_chassis_state1) );
    memset( &tx_frame_chassis_state2, 0, sizeof(tx_frame_chassis_state2) );
    memset( &rx_frame_kia_status1, 0, sizeof(rx_frame_kia_status1) );
    memset( &rx_frame_kia_status2, 0, sizeof(rx_frame_kia_status2) );
    memset( &rx_frame_kia_status3, 0, sizeof(rx_frame_kia_status3) );
    memset( &rx_frame_kia_status4, 0, sizeof(rx_frame_kia_status4) );
}


void init_pins( void )
{
    pinMode( PIN_STATUS_LED, OUTPUT );
}


void init_interfaces( void )
{
    #ifdef DEBUG
    init_serial();
    #endif

    init_can( obd_can );

    wdt_reset();

    init_can( control_can );

    wdt_reset();
}
