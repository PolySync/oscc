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
// Firmware for control of 2014 Kia Soul throttle system
// Component:
//    Arduino Uno
//    OSCC Sensor Interface Board V1
// J Hartung, 2015; E Livingston, L Buckland, D Fernández, 2016


#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "common.h"
#include "DAC_MCP49xx.h"





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

// set CAN_CS to pin 10 for CAN
#define CAN_CS 10

// ms
#define PS_CTRL_RX_WARN_TIMEOUT (2500)

// set up pins for interface with DAC (MCP4922)
#define DAC_CS              9       // Chip select pin

// Signal to ADC from car
#define SIGNAL_INPUT_A      A0

// Green wire from the torque sensor, low values
#define SIGNAL_INPUT_B      A1

// Spoof signal from DAC out to car
#define SPOOF_SIGNAL_A      A2

// Blue wire from the torque sensor, high values
#define SPOOF_SIGNAL_B      A3

// Signal interrupt (relay) for spoofed torque values
#define SPOOF_ENGAGE        6

// Threshhold to detect when a person is pressing accelerator
#define PEDAL_THRESH        1000

// Threshhold to detect when there is a discrepancy between DAC and ADC values
#define VOLTAGE_THRESH      250     // mV


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





// *****************************************************
// static declarations
// *****************************************************


// corrects for overflow condition
static void get_update_time_delta_ms(
		const uint32_t time_in,
		const uint32_t last_update_time_ms,
		uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_time_ms < time_in )
    {
		// time remainder, prior to the overflow
		( *delta_out ) = ( UINT32_MAX - time_in );

        // add time since zero
        ( *delta_out ) += last_update_time_ms;
    }
    else
    {
        // normal delta
        ( *delta_out ) = ( last_update_time_ms - time_in );
    }
}


// uses last_update_ms, corrects for overflow condition
static void get_update_time_ms(
        const uint32_t * const time_in,
        uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_ms < ( *time_in ) )
    {
        // time remainder, prior to the overflow
        ( *delta_out ) = ( UINT32_MAX - ( *time_in ) );

        // add time since zero
        ( *delta_out ) += last_update_ms;
    }
    else
    {
        // normal delta
        ( *delta_out ) = ( last_update_ms - ( *time_in ) );
    }
}


//
static void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    // debug log
    DEBUG_PRINT( "init_serial: pass" );
}


static void init_can ( void )
{
    // wait until we have initialized
    while( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
    }

    // debug log
    DEBUG_PRINT( "init_can: pass" );

}


// set up values for use in the throttle control system
uint16_t signal_L_current,      // Current measured accel sensor values
         signal_H_current,
         spoof_L_current,       // Current spoofing values
         spoof_H_current;

can_frame_s can_frame;          // CAN message structs

bool control_enable_req,
     control_enabled;

int pedal_override = 0,
    voltage_override = 0,
    loop_counter = 0;

double pedal_position_target,
       pedal_position;

uint8_t incoming_serial_byte;




/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */

// A function to enable SCM to take control
void enable_control( )
{
	// Do a quick average to smooth out the noisy data
	static int AVG_max = 20;  // Total number of samples to average over
	long sum_sensA_samples = 0;
	long sum_sensB_samples = 0;

	for ( int i = 0; i < AVG_max; i++ )
	{
		sum_sensA_samples += analogRead( SIGNAL_INPUT_A );
		sum_sensB_samples += analogRead( SIGNAL_INPUT_B );
	}

	uint16_t avg_sensA_sample = ( sum_sensA_samples / AVG_max ) << 2;
	uint16_t avg_sensB_sample = ( sum_sensB_samples / AVG_max ) << 2;

	// Write measured torque values to DAC to avoid a signal discontinuity when
    // the SCM takes over
    dac.outputA( avg_sensA_sample );
    dac.outputB( avg_sensB_sample );

	// Enable the signal interrupt relays
	digitalWrite( SPOOF_ENGAGE, HIGH );

	control_enabled = true;

	DEBUG_PRINT( "Control enabled" );

}


// A function to disable SCM control
void disable_control( )
{
	// Do a quick average to smooth out the noisy data
	static int AVG_max = 20;  // Total number of samples to average over
	long sum_sensA_samples = 0;
	long sum_sensB_samples = 0;

	for ( int i = 0; i < AVG_max; i++ )
	{
		sum_sensA_samples += analogRead(SIGNAL_INPUT_A) << 2;
		sum_sensB_samples += analogRead(SIGNAL_INPUT_B) << 2;
	}

	uint16_t avg_sensA_sample = sum_sensA_samples / AVG_max;
	uint16_t avg_sensB_sample = sum_sensB_samples / AVG_max;

	// Write measured torque values to DAC to avoid a signal discontinuity when
    //the SCM relinquishes control
    dac.outputA( avg_sensA_sample );
    dac.outputB( avg_sensB_sample );

	// Disable the signal interrupt relays
	digitalWrite( SPOOF_ENGAGE, LOW );

	control_enabled = false;

	DEBUG_PRINT( "Control disabled" );

}

void calculate_pedal_spoof( float pedal_position )
{
    // values calculated with min/max calibration curve and tuned for neutral
    // balance.vvDAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
    spoof_L_current = 819.2 * ( 0.0004 * pedal_position + 0.366 );
    spoof_H_current = 819.2 * ( 0.0008 * pedal_position + 0.732 );

    // range = 300 - ~1750
    spoof_L_current = constrain( spoof_L_current, 0, 1800 );
    // range = 600 - ~3500
    spoof_H_current = constrain( spoof_H_current, 0, 3500 );

}

//
void check_pedal_override( )
{
    if ( ( signal_L_current + signal_H_current ) / 2 > PEDAL_THRESH )
    {
        disable_control( );
        pedal_override = 1;
    }
    else
    {
        pedal_override = 0;
    }
}

//
void check_spoof_voltages(
        uint16_t spoof_L_dac,   // was A
        uint16_t spoof_H_dac,   // was B
        int loop_count
        )
{
    // only test every tenth loop
    if ( loop_count % 10 == 0 ) {
        return;
    }

    int spoof_a_adc = analogRead( SPOOF_SIGNAL_A );
    int spoof_b_adc = analogRead( SPOOF_SIGNAL_B );

    float spoof_a_adc_volts = spoof_a_adc * ( 5.0 / 1023.0 ) + 0.010;
    float spoof_b_adc_volts = spoof_b_adc * ( 5.0 / 1023.0 ) + 0.010;

    // DAC values passed in from calculate_pedal_spoof( )
    float spoof_a_dac_current_volts = spoof_H_dac * ( 5.0 / 4095.0 );
    float spoof_b_dac_current_volts = spoof_L_dac * ( 5.0 / 4095.0 );

    // fail criteria. ~ ( ± 250mV )
    if ( abs( spoof_a_adc_volts - spoof_a_dac_current_volts ) > VOLTAGE_THRESH
            && control_enabled )
    {
        DEBUG_PRINT( "* * * ERROR!!  Voltage Discrepancy on Signal A. * * *" );

        disable_control( );
        voltage_override = 1;
    }
    else
    {
        voltage_override = 0;
    }

    // fail criteria. ~ ( ± 250mV )
    if ( abs( spoof_b_adc_volts - spoof_b_dac_current_volts ) > VOLTAGE_THRESH
            && control_enabled )
    {
        DEBUG_PRINT( "* * * ERROR!!  Voltage Discrepancy on Signal B. * * *" );

        disable_control( );
        voltage_override = 1;
    }
    else
    {
        voltage_override = 0;
    }

}

// check_PWM( )


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
    tx_frame_ps_ctrl_throttle_report.dlc = 8; //TODO

    // set override flag
    if ( pedal_override == 0 && voltage_override == 0 )
    {
        data->override = 0;
    }
    else{
        data->override = 1;
    }

    //data->override = local_override;

    //// Set Pedal Command (PC)
    //data->pedal_command =

    //// Set Pedal Output (PO)
    //data->pedal_output = max()

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
    get_update_time_ms( &tx_frame_ps_ctrl_throttle_report.timestamp, &delta );

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


    bool enabled = control_data->enabled == 1;

    // enable control from the PolySync interface
    if( enabled == 1 && !control_enabled )
    {
        control_enabled = true;
        enable_control( );
    }

    // disable control from the PolySync interface
    if( enabled == 0 && control_enabled )
    {
        control_enabled = false;
        disable_control( );
    }

    rx_frame_ps_ctrl_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    pedal_position_target = control_data->pedal_command / 24 ;
    DEBUG_PRINT(pedal_position_target);

}

// A function to parse CAN data into useful variables
void handle_ready_rx_frames( void )
{

    // local vars
    can_frame_s rx_frame;

    if( CAN.checkReceive() == CAN_MSGAVAIL )
    {
        memset( &rx_frame, 0, sizeof( rx_frame ) );

        // update timestamp
        rx_frame.timestamp = last_update_ms;

        // read frame
        CAN.readMsgBufID(
                ( INT32U* ) &rx_frame.id,
                ( INT8U* ) &rx_frame.dlc,
                ( INT8U* ) rx_frame.data );

        // check for a supported frame ID
        if( rx_frame.id == PS_CTRL_THROTTLE_COMMAND_ID )
        {
            // process status1
            process_ps_ctrl_throttle_command( rx_frame.data );
        }
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
        if( control_enabled )
        {
            Serial.println( "control disabled: timeout" );
            disable_control( );
        }
    }
}


/* ====================================== */
/* ================ SETUP =============== */
/* ====================================== */

void setup()
{
    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_throttle_command,
            0,
            sizeof(rx_frame_ps_ctrl_throttle_command ) );

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

    init_serial( );

    init_can( );

    publish_ps_ctrl_throttle_report( );

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS( );

    // debug log
    DEBUG_PRINT( "init: pass" );

}


/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */

void loop()
{

    // update the global system update timestamp, ms
    last_update_ms = GET_TIMESTAMP_MS( );

    // checks for CAN frames, if yes, updates state variables
    handle_ready_rx_frames( );

    // publish all report CAN frames
    publish_timed_tx_frames( );

    // heartbeat checker??
    check_rx_timeouts( );

    // update state variables
    signal_L_current = analogRead( SIGNAL_INPUT_A ) << 2;  //10 bit to 12 bit
    signal_H_current = analogRead( SIGNAL_INPUT_B ) << 2;

    // if someone is pressing the throttle pedal, disable control
    check_pedal_override( );

    // if DAC out and ADC in voltages differ, disable control
    check_spoof_voltages( spoof_L_current, spoof_H_current, ++loop_counter );


    // now that we've set control status, do throttle if we are in control
    if ( control_enabled )
    {
        calculate_pedal_spoof( pedal_position_target );

        dac.outputA( spoof_H_current );
        dac.outputB( spoof_L_current );

    }

}
