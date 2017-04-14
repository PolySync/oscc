/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* This file is part of Open Source Car Control (OSCC).                 */
/*                                                                      */
/* OSCC is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* OSCC is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with OSCC.  If not, see <http://www.gnu.org/licenses/>.        */
/************************************************************************/

// Throttle control ECU firmware
// 2014 Kia Soul throttle system


#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "DAC_MCP49xx.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "control.h"
#include "debug.h"

#include "throttle_params.h"
#include "throttle_state.h"




// *****************************************************
// static global types/macros
// *****************************************************


// set CAN_CS to pin 10 for CAN
#define CAN_CS                          ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT         ( 250 )

// set up pins for interface with DAC (MCP4922)
#define DAC_CS                          ( 9 )       // Chip select pin

// Signal to ADC from car
#define SIGNAL_INPUT_A                  ( A0 )

// Green wire from the torque sensor, low values
#define SIGNAL_INPUT_B                  ( A1 )

// Spoof signal from DAC out to car
#define SPOOF_SIGNAL_A                  ( A2 )

// Blue wire from the torque sensor, high values
#define SPOOF_SIGNAL_B                  ( A3 )

// Signal interrupt (relay) for spoofed torque values
#define SPOOF_ENGAGE                    ( 6 )

// Threshhold to detect when there is a discrepancy between DAC and ADC values
#define VOLTAGE_THRESHOLD               ( 0.096 )     // mV



// *****************************************************
// local defined data structures
// *****************************************************

struct torque_spoof_t
{
    uint16_t low;
    uint16_t high;
};


// *****************************************************
// static global data
// *****************************************************


DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, 9 );     // DAC model, SS pin, LDAC pin

// construct the CAN shield object
MCP_CAN CAN(CAN_CS);                            // Set CS pin for the CAN shield

//
static uint32_t last_update_ms;

//
static can_frame_s rx_frame_ps_ctrl_throttle_command;

//
static can_frame_s tx_frame_ps_ctrl_throttle_report;

//
static throttle_state_s throttle_state;

//
static kia_soul_throttle_params_s throttle_params;

//
static control_state_s control_state;



// *****************************************************
// static declarations
// *****************************************************


// set up values for use in the throttle control system
uint16_t signal_L;              // Current measured accel sensor values
uint16_t signal_H;

can_frame_s can_frame;          // CAN message structs




/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */


void calculate_pedal_spoof( float pedal_target, struct torque_spoof_t* spoof )
{
    // values calculated with min/max calibration curve and tuned for neutral
    // balance.  DAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
    spoof->low = 819.2 * ( 0.0004 * pedal_target + 0.366 );
    spoof->high = 819.2 * ( 0.0008 * pedal_target + 0.732 );

    // range = 300 - ~1800
    spoof->low = constrain( spoof->low, 0, 1800 );
    // range = 600 - ~3500
    spoof->high = constrain( spoof->high, 0, 3500 );

}

//
void check_pedal_override( )
{
    if ( ( signal_L + signal_H ) / 2 > throttle_params.pedal_threshold )
    {
        disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
        throttle_state.override_flags.pedal = 1;
    }
    else
    {
        throttle_state.override_flags.pedal = 0;
    }
}




/* ====================================== */
/* =========== COMMUNICATIONS =========== */
/* ====================================== */


//
static void publish_ps_ctrl_throttle_report( void )
{
    // cast data
    ps_ctrl_throttle_report_msg * const data =
            (ps_ctrl_throttle_report_msg*) tx_frame_ps_ctrl_throttle_report.data;

    // set frame ID
    tx_frame_ps_ctrl_throttle_report.id =
            (uint32_t) (PS_CTRL_MSG_ID_THROTTLE_REPORT);

    // set DLC
    tx_frame_ps_ctrl_throttle_report.dlc = 8;

    // set override flag
    if ( ( throttle_state.override_flags.pedal == 0 ) &&
            ( throttle_state.override_flags.voltage == 0 ) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->enabled = (uint8_t) control_state.enabled;

    data->pedal_input = signal_L + signal_H;
    // Set Pedal Command (PC)
    data->pedal_command = throttle_state.pedal_position_target;

    // publish to control CAN bus
    CAN.sendMsgBuf(
            tx_frame_ps_ctrl_throttle_report.id,
            0, // standard ID (not extended)
            tx_frame_ps_ctrl_throttle_report.dlc,
            tx_frame_ps_ctrl_throttle_report.data );

    // update last publish timestamp, ms
    tx_frame_ps_ctrl_throttle_report.timestamp = last_update_ms;
}


//
static void publish_timed_tx_frames( void )
{
    // local vars
    uint32_t delta = 0;

    // get time since last publish
    get_update_time_delta_ms( tx_frame_ps_ctrl_throttle_report.timestamp, last_update_ms, &delta );

    // check publish interval
    if( delta >= PS_CTRL_THROTTLE_REPORT_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_ps_ctrl_throttle_report( );
    }
}


//
static void process_ps_ctrl_throttle_command(
        const uint8_t * const rx_frame_buffer )
{

    // cast control frame data
    const ps_ctrl_throttle_command_msg * const control_data =
            (ps_ctrl_throttle_command_msg*) rx_frame_buffer;

    // enable control from the PolySync interface
    if( ( control_data->enabled == 1 ) &&
            ( control_state.enabled == false ) &&
            ( control_state.emergency_stop == false ) )
    {
        enable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
    }

    // disable control from the PolySync interface
    if( ( control_data->enabled == 0 ) &&
            ( control_state.enabled == true ) )
    {
        disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
    }

    rx_frame_ps_ctrl_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    throttle_state.pedal_position_target = control_data->pedal_command / 24;
    DEBUG_PRINTLN( throttle_state.pedal_position_target );

}

// A function to parse CAN data into useful variables
void handle_ready_rx_frames( can_frame_s *frame )
{
    // check for a supported frame ID
    if( frame->id == PS_CTRL_THROTTLE_COMMAND_ID )
    {
        // process status1
        process_ps_ctrl_throttle_command( frame->data );
    }
}


//
static void check_rx_timeouts( void )
{
    // local vars
    uint32_t delta = 0;

    // get time since last receive
    get_update_time_delta_ms(
                       rx_frame_ps_ctrl_throttle_command.timestamp,
                       GET_TIMESTAMP_MS(),
                       &delta );

    // check rx timeout
    if( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        // disable control from the PolySync interface
        if( control_state.enabled == true )
        {
            disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
            DEBUG_PRINTLN( "Control disabled: Timeout" );
        }
    }
}


/* ====================================== */
/* ================ SETUP =============== */
/* ====================================== */

void setup( )
{
    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_throttle_command,
            0,
            sizeof( rx_frame_ps_ctrl_throttle_command ) );

    // set up pin modes
    pinMode( DAC_CS, OUTPUT );
    pinMode( SIGNAL_INPUT_A, INPUT );
    pinMode( SIGNAL_INPUT_B, INPUT );
    pinMode( SPOOF_SIGNAL_A, INPUT );
    pinMode( SPOOF_SIGNAL_B, INPUT );
    pinMode( SPOOF_ENGAGE, OUTPUT );

    // initialize the DAC board
    digitalWrite( DAC_CS, HIGH );     // Deselect DAC CS

    // Initialize relay board
    digitalWrite( SPOOF_ENGAGE, LOW );

    #ifdef DEBUG
        init_serial( );
    #endif

    init_can( CAN );

    publish_ps_ctrl_throttle_report( );

    control_state.enabled = false;

    control_state.emergency_stop = false;

    throttle_state.override_flags.pedal = 0;

    throttle_state.override_flags.voltage = 0;

    throttle_state.override_flags.voltage_spike_a = 0;

    throttle_state.override_flags.voltage_spike_b = 0;

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS( );

    // debug log
    DEBUG_PRINTLN( "init: pass" );

}


/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */

void loop()
{

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS( );

    // checks for CAN frames, if yes, updates state variables
    can_frame_s rx_frame;
    int ret = check_for_rx_frame( CAN, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
        handle_ready_rx_frames( &rx_frame );
    }

    // publish all report CAN frames
    publish_timed_tx_frames( );

    // heartbeat checker??
    check_rx_timeouts( );

    // update state variables
    signal_L = analogRead( SIGNAL_INPUT_A ) << 2;  //10 bit to 12 bit
    signal_H = analogRead( SIGNAL_INPUT_B ) << 2;

    // if someone is pressing the throttle pedal, disable control
    check_pedal_override( );

    // now that we've set control status, do throttle if we are in control
    if ( control_state.enabled == true )
    {

        struct torque_spoof_t torque_spoof;

        calculate_pedal_spoof(
                throttle_state.pedal_position_target,
                &torque_spoof );

        dac.outputA( torque_spoof.high );
        dac.outputB( torque_spoof.low );
    }

}
