/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */

/* Permission is hereby granted, free of charge, to any person */
/* obtaining a copy of this software and associated documentation */
/* files (the “Software”), to deal in the Software without */
/* restriction, including without limitation the rights to use, */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following */
/* conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR */
/* OTHER DEALINGS IN THE SOFTWARE. */
/************************************************************************/

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

#include "init.h"
#include "can_gateway_module.h"
#include "obd_can_protocol.h"
#include "communications.h"


#define STATUS_LED_ON() digitalWrite(can_gateway_module.pins.status_led, HIGH);
#define STATUS_LED_OFF() digitalWrite(can_gateway_module.pins.status_led, LOW);


static kia_soul_can_gateway_module_s can_gateway_module;
static MCP_CAN obd_can( can_gateway_module.pins.obd_can_cs );
static MCP_CAN control_can( can_gateway_module.pins.control_can_cs );
static can_frame_s tx_frame_heartbeat;
static can_frame_s tx_frame_chassis_state1;
static can_frame_s tx_frame_chassis_state2;
static can_frame_s rx_frame_kia_status1;
static can_frame_s rx_frame_kia_status2;
static can_frame_s rx_frame_kia_status3;
static can_frame_s rx_frame_kia_status4;


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

    init_pins( &can_gateway_module );

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
    publish_heartbeat_frame( &tx_frame_heartbeat, control_can);

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
         handle_ready_rx_frames(
            &rx_frame_kia_status1,
            &rx_frame_kia_status2,
            &rx_frame_kia_status3,
            &rx_frame_kia_status4,
            &tx_frame_chassis_state1,
            &tx_frame_chassis_state2,
            &tx_frame_heartbeat,
            &rx_frame );
    }

    publish_timed_tx_frames(
        &tx_frame_heartbeat,
        &tx_frame_chassis_state1,
        &tx_frame_chassis_state2,
        control_can);

    check_rx_timeouts(
        &rx_frame_kia_status1,
        &rx_frame_kia_status2,
        &rx_frame_kia_status3,
        &rx_frame_kia_status4,
        &tx_frame_chassis_state1,
        &tx_frame_heartbeat);
}
