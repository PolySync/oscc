/**
 * @file can_gateway.ino
 * @brief CAN Gateway Module Source.
 *
 * Board: Arduino Uno
 * Arduino Build/Version: 1.6.7 linux-x86_64
 *
 * @warning Requires watchdog reset support in the bootloader, which is NOT supported
 * in all Arduino bootloaders.
 *
 */


#include <stdint.h>
#include <avr/wdt.h>
#include <SPI.h>

#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "init.h"
#include "obd_can_protocol.h"
#include "communications.h"


#define STATUS_LED_ON() digitalWrite( PIN_STATUS_LED, HIGH );
#define STATUS_LED_OFF() digitalWrite( PIN_STATUS_LED, LOW );


void setup( void )
{
    // zero
    memset( &tx_frame_heartbeat, 0, sizeof(tx_frame_heartbeat) );
    memset( &tx_frame_chassis_state1, 0, sizeof(tx_frame_chassis_state1) );
    memset( &tx_frame_chassis_state2, 0, sizeof(tx_frame_chassis_state2) );
    memset( &rx_frame_kia_status1, 0, sizeof(rx_frame_kia_status1) );
    memset( &rx_frame_kia_status2, 0, sizeof(rx_frame_kia_status2) );
    memset( &rx_frame_kia_status3, 0, sizeof(rx_frame_kia_status3) );
    memset( &rx_frame_kia_status4, 0, sizeof(rx_frame_kia_status4) );

    init_pins( );

    SET_STATE( tx_frame_heartbeat.data, OSCC_HEARTBEAT_STATE_INIT );

    STATUS_LED_OFF();

    // disable watchdog
    wdt_disable();

    // enable watchdog, reset after 120 ms
    wdt_enable( WDTO_120MS );

    // reset watchdog
    wdt_reset();

    #ifdef DEBUG
        init_serial();
    #endif

    init_can( obd_can );

    // reset watchdog
    wdt_reset();

    init_can( control_can );

    // reset watchdog
    wdt_reset();

    // publish heartbeat showing that we are initializing
    publish_heartbeat_frame( );

    // wait a little so we can offset CAN frame Tx timestamps
    SLEEP_MS(5);

    // offset CAN frame Tx timestamp
    // so we don't publish at the same time as the heartbeat frame
    tx_frame_chassis_state1.timestamp = GET_TIMESTAMP_MS();

    // wait a little so we can offset CAN frame Tx timestamps
    SLEEP_MS(5);

    // offset CAN frame Tx timestamp
    // so we dont publish at the same time as the chassis1 frame
    tx_frame_chassis_state2.timestamp = GET_TIMESTAMP_MS();

    // reset watchdog
    wdt_reset();

    rx_frame_kia_status1.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status2.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status3.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status4.timestamp = GET_TIMESTAMP_MS();

    STATUS_LED_ON();

    SET_STATE( tx_frame_heartbeat.data, OSCC_HEARTBEAT_STATE_OK );

    DEBUG_PRINTLN( "init: pass" );
}


void loop( void )
{
    // reset watchdog
    wdt_reset();

    can_frame_s rx_frame;
    int ret = check_for_rx_frame( obd_can, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
         handle_ready_rx_frames( &rx_frame );
    }

    publish_timed_tx_frames( );

    check_rx_timeouts( );
}
