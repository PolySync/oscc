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

// Steering control ECU firmware
// Firmware for control of 2014 Kia Soul Motor Driven Power Steering (MDPS) system
// Components:
//    Arduino Uno
//    OSCC Sensor Interface Board V1
// J Hartung, 2015; E Livingston, L Buckland, D Fernández, 2016


#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "current_control_state.h"
#include "PID.h"
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
#define CAN_CS                          ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT         ( 2500 )

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

// Threshhold to detect when a person is turning the steering wheel
#define STEERING_WHEEL_CUTOFF_THRESHOLD ( 3000 )

// Threshhold to detect when there is a discrepancy between DAC and ADC values
#define VOLTAGE_THRESH                  ( 200 )     // mV




// *****************************************************
// static structures
// *****************************************************


DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, 9 );     // DAC model, SS pin, LDAC pin

// Construct the CAN shield object
MCP_CAN CAN(CAN_CS);                            // Set CS pin for the CAN shield


//
static can_frame_s rx_frame_ps_ctrl_steering_command;


//
static can_frame_s tx_frame_ps_ctrl_steering_report;


//
static current_control_state current_ctrl_state;


//
static PID pidParams;


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
		(*delta_out) = (UINT32_MAX - time_in);

        // add time since zero
        (*delta_out) += last_update_time_ms;
    }
    else
    {
        // normal delta
        (*delta_out) = ( last_update_time_ms - time_in );
    }
}


//
static void init_serial( void )
{
    Serial.begin( SERIAL_BAUD );

    // debug log
    DEBUG_PRINT( "init_serial: pass" );
}


//
static void init_can ( void )
{
    // wait until we have initialized
    while( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        DEBUG_PRINT( "init_can: retrying" );
        delay( CAN_INIT_RETRY_DELAY );
    }

    // debug log
    DEBUG_PRINT( "init_can: pass" );
}


// set up values for use in the steering control system
static uint16_t voltage_override = 0;
static uint16_t test_countdown = 0;




/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */



// A function to enable SCM to take control
void enable_control( )
{
	// Do a quick average to smooth out the noisy data
	static uint16_t n_samples = 20;  // Total number of samples to average over
	long sum_sensA_samples = 0;
	long sum_sensB_samples = 0;

	for ( int i = 0; i < n_samples; i++ )
	{
		sum_sensA_samples += analogRead( SIGNAL_INPUT_A );
		sum_sensB_samples += analogRead( SIGNAL_INPUT_B );
	}

	uint16_t avg_sensA_sample = ( sum_sensA_samples / n_samples ) << 2;
	uint16_t avg_sensB_sample = ( sum_sensB_samples / n_samples ) << 2;

	// Write measured torque values to DAC to avoid a signal discontinuity when
    // the SCM takes over
    dac.outputA( avg_sensA_sample );
    dac.outputB( avg_sensB_sample );

	// Enable the signal interrupt relays
	digitalWrite( SPOOF_ENGAGE, HIGH );

	current_ctrl_state.control_enabled = true;

	DEBUG_PRINT( "Control enabled" );

}


// A function to disable SCM control
void disable_control( )
{
	// Do a quick average to smooth out the noisy data
	static uint16_t n_samples = 20;  // Total number of samples to average over
	long sum_sensA_samples = 0;
	long sum_sensB_samples = 0;

	for ( int i = 0; i < n_samples; i++ )
	{
		sum_sensA_samples += analogRead( SIGNAL_INPUT_A ) << 2;
		sum_sensB_samples += analogRead( SIGNAL_INPUT_B ) << 2;
	}

	uint16_t avg_sensA_sample = sum_sensA_samples / n_samples;
	uint16_t avg_sensB_sample = sum_sensB_samples / n_samples;

	// Write measured torque values to DAC to avoid a signal discontinuity when
    // the SCM relinquishes control
    dac.outputA( avg_sensA_sample );
    dac.outputB( avg_sensB_sample );

	// Disable the signal interrupt relays
	digitalWrite( SPOOF_ENGAGE, LOW );

	current_ctrl_state.control_enabled = false;

	DEBUG_PRINT( "Control disabled" );
}


//
void calculate_torque_spoof(
        float torque,
        uint16_t * Spoof_L,
        uint16_t * Spoof_H )
{
	// Values calculated with min/max calibration curve and tuned for neutral
    // balance.  DAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
	( *Spoof_L ) = 819.2 * ( 0.0008 * torque + 2.26 );
	( *Spoof_H ) = 819.2 * ( -0.0008 * torque + 2.5 );
}


//
void check_spoof_voltages(
        uint16_t * spoof_L_dac,   // was A
        uint16_t * spoof_H_dac    // was B
        )
{

    int spoof_a_adc = analogRead( SPOOF_SIGNAL_A );
    int spoof_b_adc = analogRead( SPOOF_SIGNAL_B );

    float spoof_a_adc_volts = spoof_a_adc * ( 5.0 / 1023.0 ) + 0.010;
    float spoof_b_adc_volts = spoof_b_adc * ( 5.0 / 1023.0 ) + 0.010;

    // DAC values passed in from calculate_torque_spoof( )
    float spoof_a_dac_current_volts = ( *spoof_H_dac ) * ( 5.0 / 4095.0 );
    float spoof_b_dac_current_volts = ( *spoof_L_dac ) * ( 5.0 / 4095.0 );

    // fail criteria. ~ ( ± 200mV )
    if ( abs( spoof_a_adc_volts - spoof_a_dac_current_volts ) > VOLTAGE_THRESH )
    {
        DEBUG_PRINT( "* * * ERROR!!  Voltage Discrepancy on Signal A. * * *" );

        disable_control( );
        voltage_override = 1;
    }
    else
    {
        voltage_override = 0;
    }

    // fail criteria. ~ ( ± 200mV )
    if ( abs( spoof_b_adc_volts - spoof_b_dac_current_volts ) > VOLTAGE_THRESH )
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




/* ====================================== */
/* =========== COMMUNICATIONS =========== */
/* ====================================== */


//
static void publish_ps_ctrl_steering_report( void )
{
    // cast data
    ps_ctrl_steering_report_msg * const data =
            (ps_ctrl_steering_report_msg*) tx_frame_ps_ctrl_steering_report.data;

    // Set frame ID
    tx_frame_ps_ctrl_steering_report.id =
            (uint32_t) (PS_CTRL_MSG_ID_STEERING_REPORT);

    // Set DLC
    tx_frame_ps_ctrl_steering_report.dlc = 8;

    // Steering Wheel Angle
    data->angle = current_ctrl_state.current_steering_angle;

    // Update last publish timestamp, ms
    tx_frame_ps_ctrl_steering_report.timestamp = GET_TIMESTAMP_MS( );

    // Publish to control CAN bus
    CAN.sendMsgBuf(
            tx_frame_ps_ctrl_steering_report.id,
            0, // standard ID (not extended)
            tx_frame_ps_ctrl_steering_report.dlc,
            tx_frame_ps_ctrl_steering_report.data );
}


//
static void publish_timed_tx_frames( void )
{

    // Local vars
    uint32_t delta = 0;

    // Get time since last publish
    get_update_time_delta_ms(
			tx_frame_ps_ctrl_steering_report.timestamp,
			GET_TIMESTAMP_MS(),
			&delta );

    // check publish interval
    if( delta >= PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL )
    {
        // publish frame, update timestamp
        publish_ps_ctrl_steering_report( );
    }
}


//
static void process_ps_ctrl_steering_command(
        const uint8_t * const rx_frame_buffer )
{
    // Cast control frame data
    const ps_ctrl_steering_command_msg * const control_data =
            (ps_ctrl_steering_command_msg*) rx_frame_buffer;

    current_ctrl_state.commanded_steering_angle =
            control_data->steering_wheel_angle_command / 9.0 ;

    current_ctrl_state.steering_angle_rate_max =
            control_data->steering_wheel_max_velocity * 9.0;

    bool enabled = control_data->enabled == 1;

    // Enable control from the PolySync interface
    if(     enabled == 1 &&
            !current_ctrl_state.control_enabled &&
            !current_ctrl_state.emergency_stop )
    {
        enable_control( );
    }

    // Disable control from the PolySync interface
    if( enabled == 0 && current_ctrl_state.control_enabled )
    {
        disable_control( );
    }

	rx_frame_ps_ctrl_steering_command.timestamp = GET_TIMESTAMP_MS( );
}


//
static void process_psvc_chassis_state1( const uint8_t * const rx_frame_buffer )
{
    const psvc_chassis_state1_data_s * const chassis_data =
        (psvc_chassis_state1_data_s*) rx_frame_buffer;

    int16_t raw_angle = chassis_data->steering_wheel_angle;

    current_ctrl_state.current_steering_angle = float( raw_angle ) * 0.0076294;

	// Convert from 40 degree range to 470 degree range in 1 deg increments
	current_ctrl_state.current_steering_angle *= 11.7;

}


// A function to parse CAN data into useful variables
void handle_ready_rx_frames(void)
{
    // local vars
    can_frame_s rx_frame;

    if( CAN.checkReceive( ) == CAN_MSGAVAIL )
    {
        memset( &rx_frame, 0, sizeof( rx_frame ) );

        // read frame
        CAN.readMsgBufID(
                ( INT32U* ) &rx_frame.id,
                ( INT8U* ) &rx_frame.dlc,
                ( INT8U* ) rx_frame.data );

        // check for a supported frame ID
        if( rx_frame.id == PS_CTRL_MSG_ID_STEERING_COMMAND )
        {
            // process steering commmand
            process_ps_ctrl_steering_command( rx_frame.data );
        }


        if( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            // process state1
            process_psvc_chassis_state1( rx_frame.data );

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
			rx_frame_ps_ctrl_steering_command.timestamp,
			GET_TIMESTAMP_MS(),
			&delta );

    // check rx timeout
    if( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        // disable control from the PolySync interface
        if( current_ctrl_state.control_enabled )
        {
            disable_control( );
        }
    }
}




/* ====================================== */
/* ================ SETUP =============== */
/* ====================================== */


//
void setup()
{
    // zero
    memset( &rx_frame_ps_ctrl_steering_command,
            0,
            sizeof( rx_frame_ps_ctrl_steering_command ) );

    // Set up pin modes
    pinMode( DAC_CS, OUTPUT );
    pinMode( SIGNAL_INPUT_A, INPUT );
    pinMode( SIGNAL_INPUT_B, INPUT );
    pinMode( SPOOF_SIGNAL_A, INPUT );
    pinMode( SPOOF_SIGNAL_B, INPUT );
    pinMode( SPOOF_ENGAGE, OUTPUT );

    // Initialize the DAC board
    digitalWrite( DAC_CS, HIGH );     // Deselect DAC CS

    // Initialize relay board
    digitalWrite( SPOOF_ENGAGE, LOW );

    init_serial( );

    init_can( );

    publish_ps_ctrl_steering_report( );

    current_ctrl_state.control_enabled = false;

    current_ctrl_state.emergency_stop = false;

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_steering_command.timestamp = GET_TIMESTAMP_MS( );

    pid_zeroize( &pidParams );

    // debug log
    DEBUG_PRINT( "init: pass" );
}




/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */


//
void loop()
{
    // checks for CAN frames, if yes, updates state variables
    handle_ready_rx_frames( );

    // publish all report CAN frames
    publish_timed_tx_frames( );

    // check all timeouts
    check_rx_timeouts( );

    // Calculate a delta t
    long unsigned int currMicros = micros( );  // Fast loop, needs more prec

    unsigned int deltaT = currMicros - current_ctrl_state.lastMicros;

    if( deltaT > 50000 )
    {
        current_ctrl_state.lastMicros = currMicros;

        // Now that we've set control status, do steering if we are in control
        if( current_ctrl_state.control_enabled )
	    {
            // Calculate steering angle rates
            double steeringAngleRate = (
                    current_ctrl_state.current_steering_angle -
                    current_ctrl_state.steering_angle_last ) / 0.05;  //deg/us

            double steeringAngleRateTarget = (
                    current_ctrl_state.commanded_steering_angle -
                    current_ctrl_state.current_steering_angle ) / 0.05; //deg/us

            // Save for next time
            current_ctrl_state.steering_angle_last =
                    current_ctrl_state.current_steering_angle;

            // Set saturation limits for steering wheel rotation speed
            if(     steeringAngleRateTarget >=
                    current_ctrl_state.steering_angle_rate_max )
            {
                steeringAngleRateTarget =
                        current_ctrl_state.steering_angle_rate_max;
            }

            if(     steeringAngleRateTarget <=
                    -current_ctrl_state.steering_angle_rate_max )
            {
                steeringAngleRateTarget =
                        -current_ctrl_state.steering_angle_rate_max;
            }

            pidParams.derivative_gain = current_ctrl_state.SA_Kd;
            pidParams.proportional_gain = current_ctrl_state.SA_Kp;
            pidParams.integral_gain = current_ctrl_state.SA_Ki;

            int ret = pid_update(
                    &pidParams,
                    steeringAngleRateTarget - steeringAngleRate,
                    0.050 );

            if( ret == PID_SUCCESS )
            {
                test_countdown += 1;

                uint16_t spoof_L;
                uint16_t spoof_H;

                double control = pidParams.control;

                // constrain to min/max
                control = m_constrain(
                        (float) ( control ),
                        (float) -1500.0f,
                        (float) 1500.0f );

                calculate_torque_spoof( control, &spoof_L, &spoof_H );

                dac.outputA( spoof_H );
                dac.outputB( spoof_L );

                // if DAC out and ADC in voltages differ, disable control
                // only test every tenth loop
                if ( test_countdown >= 10 ) {

                    test_countdown = 0;
                    check_spoof_voltages( &spoof_L, &spoof_H );
                }
            }
        }
        else
        {
            pid_zeroize( &pidParams );
        }

    }
}
