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
#include "can_frame.h"
#include "kia_obd_can.h"
#include "common.h"
#include "control_protocol_can.h"




// *****************************************************
// static global types/macros
// *****************************************************


#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
    #warning "PSYNC_DEBUG_FLAG defined"
    #define DEBUG_PRINT(x)  Serial.println(x)
#else
    #define DEBUG_PRINT(x)
#endif


//
#define PSVC_VGM_NODE_ID (0x10)


//
#define PSVC_VGM_HARDWARE_VERSION (0x1)


//
#define PSVC_VGM_FIRMWARE_VERSION (0x1)


//
#define PIN_OBD_CAN_CS (9)


//
#define PIN_CONTROL_CAN_CS (10)


//
#define PIN_STATUS_LED (13)


//
#define STATUS_LED_ON() digitalWrite(PIN_STATUS_LED, HIGH);


//
#define STATUS_LED_OFF() digitalWrite(PIN_STATUS_LED, LOW);


//
#define SET_WARNING(x) (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->warning_register |= ((uint16_t) x))


//
#define CLEAR_WARNING(x) (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->warning_register &= ~((uint16_t) x))


//
#define SET_ERROR(x) (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->error_register |= ((uint16_t) x))


//
#define CLEAR_ERROR(x) (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->error_register &= ~((uint16_t) x))


//
#define SET_STATE(x) (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->state = ((uint8_t) x))


//
#define GET_STATE() (((psvc_heartbeat_data_s*) &tx_frame_heartbeat.data)->state)


//
#define SET_CHASSIS_FLAG(x) (((psvc_chassis_state1_data_s*) &tx_frame_chassis_state1.data)->flags |= ((uint8_t) x))


//
#define CLEAR_CHASSIS_FLAG(x) (((psvc_chassis_state1_data_s*) &tx_frame_chassis_state1.data)->flags &= ~((uint8_t) x))




// *****************************************************
// static global data
// *****************************************************

//
static uint32_t last_update_ms;


//
static MCP_CAN obd_can( PIN_OBD_CAN_CS );


//
static MCP_CAN control_can( PIN_CONTROL_CAN_CS );


// timestamp is last publish timestamp in milliseconds
static can_frame_s tx_frame_heartbeat;


//
static can_frame_s tx_frame_chassis_state1;


//
static can_frame_s tx_frame_chassis_state2;


//
static can_frame_s rx_frame_kia_status1;


//
static can_frame_s rx_frame_kia_status2;


//
static can_frame_s rx_frame_kia_status3;


//
static can_frame_s rx_frame_kia_status4;




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************

// uses last_update_ms, corrects for overflow condition
static void get_update_time_delta_ms(
        const uint32_t * const time_in,
        uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_ms < (*time_in) )
    {
        // time remainder, prior to the overflow
        (*delta_out) = (UINT32_MAX - (*time_in));

        // add time since zero
        (*delta_out) += last_update_ms;
    }
    else
    {
        // normal delta
        (*delta_out) = (last_update_ms - (*time_in));
    }
}


//
static void init_serial( void )
{
    // disable serial
    Serial.end();

    // init if debugging
#ifdef PSYNC_DEBUG_FLAG
    Serial.begin( SERIAL_BAUD );
#endif

    // debug log
    DEBUG_PRINT( "init_serial: pass" );
}


//
static void init_obd_can( void )
{
    // wait until we have initialized
    // watchdog will reset if we take too long
    while( obd_can.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }

    // debug log
    DEBUG_PRINT( "init_obd_can: pass" );
}


//
static void init_control_can( void )
{
    // wait until we have initialized
    // watchdog will reset if we take too long
    while( control_can.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }

    // debug log
    DEBUG_PRINT( "init_control_can: pass" );
}


//
static void publish_heartbeat_frame( void )
{
    // cast data
    psvc_heartbeat_data_s * const data =
            (psvc_heartbeat_data_s*) tx_frame_heartbeat.data;

    // set frame ID
    tx_frame_heartbeat.id = (uint32_t) (PSVC_HEARTBEAT_MSG_BASE_ID + PSVC_VGM_NODE_ID);

    // set DLC
    tx_frame_heartbeat.dlc = PSVC_HEARTBEAT_MSG_DLC;

    // set hardware version
    data->hardware_version = PSVC_VGM_HARDWARE_VERSION;

    // set firmware version
    data->firmware_version = PSVC_VGM_FIRMWARE_VERSION;
	
    // publish to control CAN bus
    control_can.sendMsgBuf(
            tx_frame_heartbeat.id,
            0, // standard ID (not extended)
            tx_frame_heartbeat.dlc,
            tx_frame_heartbeat.data );

    // update last publish timestamp, ms
    tx_frame_heartbeat.timestamp = last_update_ms;
}


//
static void publish_chassis_state1_frame( void )
{
    // set frame ID
    tx_frame_chassis_state1.id = (uint32_t) PSVC_CHASSIS_STATE1_MSG_ID;

    // set DLC
    tx_frame_chassis_state1.dlc = PSVC_CHASSIS_STATE1_MSG_DLC;

    // publish to control CAN bus
    control_can.sendMsgBuf(
            tx_frame_chassis_state1.id,
            0, // standard ID (not extended)
            tx_frame_chassis_state1.dlc,
            tx_frame_chassis_state1.data );

    // update last publish timestamp, ms
    tx_frame_chassis_state1.timestamp = last_update_ms;
}


//
static void publish_chassis_state2_frame( void )
{
    // set frame ID
    tx_frame_chassis_state2.id = (uint32_t) PSVC_CHASSIS_STATE2_MSG_ID;

    // set DLC
    tx_frame_chassis_state2.dlc = PSVC_CHASSIS_STATE2_MSG_DLC;

    // publish to control CAN bus
    control_can.sendMsgBuf(
            tx_frame_chassis_state2.id,
            0, // standard ID (not extended)
            tx_frame_chassis_state2.dlc,
            tx_frame_chassis_state2.data );

    // update last publish timestamp, ms
    tx_frame_chassis_state2.timestamp = last_update_ms;
}


//
static void publish_timed_tx_frames( void )
{
    // local vars
    uint32_t delta = 0;


    // get time since last publish
    get_update_time_delta_ms( &tx_frame_heartbeat.timestamp, &delta );

    // check publish interval
    if( delta >= PSVC_HEARTBEAT_MSG_TX_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_heartbeat_frame();
    }

    // get time since last publish
    get_update_time_delta_ms( &tx_frame_chassis_state1.timestamp, &delta );

    // check publish interval
    if( delta >= PSVC_CHASSIS_STATE1_MSG_TX_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_chassis_state1_frame();
    }

    // get time since last publish
    get_update_time_delta_ms( &tx_frame_chassis_state2.timestamp, &delta );

    // check publish interval
    if( delta >= PSVC_CHASSIS_STATE2_MSG_TX_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_chassis_state2_frame();
    }
}


//
static void process_kia_status1( const can_frame_s * const rx_frame )
{
    // local vars
    int32_t s32 = 0;


    // copy to global frame
    memcpy(
            &rx_frame_kia_status1,
            rx_frame,
            sizeof(*rx_frame) );

    // cast Kia frame data
    const kia_obd_status1_data_s * const kia_data =
            (kia_obd_status1_data_s*) rx_frame_kia_status1.data;

    // cast control frame data
    psvc_chassis_state1_data_s * const control_data =
            (psvc_chassis_state1_data_s*) tx_frame_chassis_state1.data;

    // clear timeout warning
    CLEAR_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT );

    // validate steering wheel angle and rate
    SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID );
    //SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );

    // invalidate steering wheel angle rate
    CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    control_data->steering_wheel_angle_rate = 0;

    // convert steering wheel angle
    s32 = (int32_t) kia_data->steering_angle;
    control_data->steering_wheel_angle = (int16_t) s32;

#warning "TODO - steering wheel angle convertion and rate calculation"
}


//
static void process_kia_status2( const can_frame_s * const rx_frame )
{
    // local vars
    int32_t s32 = 0;


    // copy to global frame
    memcpy(
            &rx_frame_kia_status2,
            rx_frame,
            sizeof(*rx_frame) );

    // cast Kia frame data
    const kia_obd_status2_data_s * const kia_data =
            (kia_obd_status2_data_s*) rx_frame_kia_status2.data;

    // cast control frame data
    psvc_chassis_state2_data_s * const control_data =
            (psvc_chassis_state2_data_s*) tx_frame_chassis_state2.data;

    // clear timeout warning
    CLEAR_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT );

    // validate wheel speed
    SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID );

#warning "TODO - wheel speed unit convertion"
    // convert left front wheel speed
    s32 = (int32_t) kia_data->wheel_speed_lf;
    control_data->wheel_speed_lf = (int16_t) s32;

    // convert right front wheel speed
    s32 = (int32_t) kia_data->wheel_speed_rf;
    control_data->wheel_speed_rf = (int16_t) s32;

    // convert left rear wheel speed
    s32 = (int32_t) kia_data->wheel_speed_lr;
    control_data->wheel_speed_lr = (int16_t) s32;

    // convert right rear wheel speed
    s32 = (int32_t) kia_data->wheel_speed_rr;
    control_data->wheel_speed_rr = (int16_t) s32;
}


//
static void process_kia_status3( const can_frame_s * const rx_frame )
{
    // local vars
    int16_t s16 = 0;


    // copy to global frame
    memcpy(
            &rx_frame_kia_status3,
            rx_frame,
            sizeof(*rx_frame) );

    // cast Kia frame data
    const kia_obd_status3_data_s * const kia_data =
            (kia_obd_status3_data_s*) rx_frame_kia_status3.data;

    // cast control frame data
    psvc_chassis_state1_data_s * const control_data =
            (psvc_chassis_state1_data_s*) tx_frame_chassis_state1.data;

    // clear timeout warning
    CLEAR_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT );

    // validate brake pressure
    SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID );

#warning "TODO - brake pressure unit convertion"
    // convert
    s16 = (int16_t) kia_data->master_cylinder_pressure;
    control_data->brake_pressure = (int16_t) s16;
}


//
static void process_kia_status4( const can_frame_s * const rx_frame )
{
    // copy to global frame
    memcpy(
            &rx_frame_kia_status4,
            rx_frame,
            sizeof(*rx_frame) );

    // cast Kia frame data
    const kia_obd_status4_data_s * const kia_data =
            (kia_obd_status4_data_s*) rx_frame_kia_status4.data;

    // cast control frame data
    psvc_chassis_state1_data_s * const control_data =
            (psvc_chassis_state1_data_s*) tx_frame_chassis_state1.data;

    // clear timeout warning
    CLEAR_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT );

    // clear signals
    CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
    CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
    CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON );

    //
    if( kia_data->turn_signal_flags == KIA_CCAN_STATUS4_TURN_SIGNAL_LEFT )
    {
        SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
    }

    //
    if( kia_data->turn_signal_flags == KIA_CCAN_STATUS4_TURN_SIGNAL_RIGHT )
    {
        SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
    }
}


//
static void handle_ready_rx_frames( void )
{
    // local vars
    can_frame_s rx_frame;


    // check if OBD CAN frame is ready to be read
    if( obd_can.checkReceive() == CAN_MSGAVAIL )
    {
        // zero
        memset( &rx_frame, 0, sizeof(rx_frame) );

        // update timestamp
        rx_frame.timestamp = last_update_ms;

        // read frame
        obd_can.readMsgBufID(
                (INT32U*) &rx_frame.id,
                (INT8U*) &rx_frame.dlc,
                (INT8U*) rx_frame.data );

        // check for a supported frame ID
        if( rx_frame.id == KIA_CCAN_STATUS1_MESSAGE_ID )
        {
            // process status1
            process_kia_status1( &rx_frame );
        }
        else if( rx_frame.id == KIA_CCAN_STATUS2_MESSAGE_ID )
        {
            // process status2
            process_kia_status2( &rx_frame );
        }
        else if( rx_frame.id == KIA_CCAN_STATUS3_MESSAGE_ID )
        {
            // process status3
            process_kia_status3( &rx_frame );
        }
        else if( rx_frame.id == KIA_CCAN_STATUS4_MESSAGE_ID )
        {
            // process status4
            process_kia_status4( &rx_frame );
        }
    }
}


//
static void check_rx_timeouts( void )
{
    // local vars
    uint32_t delta = 0;


    // get time since last receive
    get_update_time_delta_ms( &rx_frame_kia_status1.timestamp, &delta );

    // check Rx timeout
    if( delta >= KIA_CCAN_STATUS1_RX_WARN_TIMEOUT )
    {
        // set timeout warning
        SET_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT );

        // invalidate steering wheel angle and rate
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID );
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    }

    // get time since last receive
    get_update_time_delta_ms( &rx_frame_kia_status2.timestamp, &delta );

    // check Rx timeout
    if( delta >= KIA_CCAN_STATUS2_RX_WARN_TIMEOUT )
    {
        // set timeout warning
        SET_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT );

        // invalidate wheel speed
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID );
    }

    // get time since last receive
    get_update_time_delta_ms( &rx_frame_kia_status3.timestamp, &delta );

    // check Rx timeout
    if( delta >= KIA_CCAN_STATUS3_RX_WARN_TIMEOUT )
    {
        // set timeout warning
        SET_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT );

        // invalidate brake pressure
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID );
    }

    // get time since last receive
    get_update_time_delta_ms( &rx_frame_kia_status4.timestamp, &delta );

    // check Rx timeout
    if( delta >= KIA_CCAN_STATUS4_RX_WARN_TIMEOUT )
    {
        // set timeout warning
        SET_WARNING( PSVC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT );

        // clear signals
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON );
    }
}




// *****************************************************
// public definitions
// *****************************************************

//
void setup( void )
{
    // zero
    last_update_ms = 0;
    memset( &tx_frame_heartbeat, 0, sizeof(tx_frame_heartbeat) );
    memset( &tx_frame_chassis_state1, 0, sizeof(tx_frame_chassis_state1) );
    memset( &tx_frame_chassis_state2, 0, sizeof(tx_frame_chassis_state2) );
    memset( &rx_frame_kia_status1, 0, sizeof(rx_frame_kia_status1) );
    memset( &rx_frame_kia_status2, 0, sizeof(rx_frame_kia_status2) );
    memset( &rx_frame_kia_status3, 0, sizeof(rx_frame_kia_status3) );
    memset( &rx_frame_kia_status4, 0, sizeof(rx_frame_kia_status4) );

    // set state: init
    SET_STATE( PSVC_HEARTBEAT_STATE_INIT );

    // init LED status pin
    pinMode( PIN_STATUS_LED, OUTPUT );

    // disable status LED
    STATUS_LED_OFF();

    // disable watchdog
    wdt_disable();

    // enable watchdog, reset after 120 ms
    wdt_enable( WDTO_120MS );

    // reset watchdog
    wdt_reset();

    // init serial
    init_serial();

    // init OBD CAN
    init_obd_can();

    // reset watchdog
    wdt_reset();

    // init control CAN
    init_control_can();

    // reset watchdog
    wdt_reset();

    // publish heartbeat showing that we are initializing
    publish_heartbeat_frame();

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

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_kia_status1.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status2.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status3.timestamp = GET_TIMESTAMP_MS();
    rx_frame_kia_status4.timestamp = GET_TIMESTAMP_MS();

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS();

    // enable status LED
    STATUS_LED_ON();

    // set state: ok
    SET_STATE( PSVC_HEARTBEAT_STATE_OK );

    // debug log
    DEBUG_PRINT( "init: pass" );
}


//
void loop( void )
{
    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS();

    // reset watchdog
    wdt_reset();

    // read and process any available OBD CAN Rx frames
    handle_ready_rx_frames();

    // publish all CAN Tx frames according to their individual Tx rates
    publish_timed_tx_frames();

    // update/check all CAN Rx frame timeouts
    check_rx_timeouts();
}
