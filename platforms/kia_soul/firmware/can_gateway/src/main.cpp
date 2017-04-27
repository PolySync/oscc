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


#include <Arduino.h>
#include <stdint.h>
#include <avr/wdt.h>
#include <SPI.h>

#include "arduino_init.h"
#include "mcp_can.h"
#include "gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "init.h"
#include "obd_can_protocol.h"
#include "communications.h"


int main( void )
{
    init_arduino( );

    init_globals( );

    SET_STATE( tx_heartbeat.data, OSCC_REPORT_HEARTBEAT_STATE_INIT );

    // disable watchdog
    wdt_disable();

    // enable watchdog, reset after 120 ms
    wdt_enable( WDTO_120MS );

    // reset watchdog
    wdt_reset();

    init_interfaces( );

    // publish heartbeat showing that we are initializing
    publish_heartbeat_frame( );

    // wait a little so we can offset CAN frame Tx timestamps
    SLEEP_MS(5);

    // offset CAN frame Tx timestamp
    // so we don't publish at the same time as the heartbeat frame
    tx_chassis_state_1.timestamp = GET_TIMESTAMP_MS();

    // wait a little so we can offset CAN frame Tx timestamps
    SLEEP_MS(5);

    // offset CAN frame Tx timestamp
    // so we dont publish at the same time as the chassis1 frame
    tx_chassis_state_2.timestamp = GET_TIMESTAMP_MS();

    // reset watchdog
    wdt_reset();

    SET_STATE( tx_heartbeat.data, OSCC_REPORT_HEARTBEAT_STATE_OK );

    DEBUG_PRINTLN( "init: pass" );


    while( true )
    {
        // reset watchdog
        wdt_reset();

        can_frame_s rx_frame;
        can_status_t ret = check_for_rx_frame( obd_can, &rx_frame );

        if( ret == CAN_RX_FRAME_AVAILABLE )
        {
            process_rx_frame( &rx_frame );
        }

        publish_reports( );

        check_for_command_timeout( );
    }

    return 0;
}
