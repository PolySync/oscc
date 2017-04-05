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
// 2014 Kia Soul Motor Driven Power Steering (MDPS) system


#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "current_control_state.h"
#include "PID.h"
#include "common.h"
#include "DAC_MCP49xx.h"
#include "steering_control.h"
#include "test_steering_control.h"
#include "globals.h"


// *****************************************************
// static global types/macros
// *****************************************************

#define PSYNC_DEBUG_FLAG ( true )

#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT( x )  Serial.println( x )
    #define STATIC
#else
    #define DEBUG_PRINT( x )
    #define STATIC static
#endif

bool test_code = false;

/* ====================================== */
/* ================ SETUP =============== */
/* ====================================== */

// *****************************************************
// Function:    setup
//
// Purpose:     Initialize and clear all global data
//              Set up hardware
//              Initialize control loop variables
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void setup( )
{
    memset( &rx_frame_ps_ctrl_steering_command,
            0,
            sizeof(rx_frame_ps_ctrl_steering_command) );

    // Set the direction for analog pins

    pinMode( DAC_CS, OUTPUT );
    pinMode( SIGNAL_INPUT_A, INPUT );
    pinMode( SIGNAL_INPUT_B, INPUT );
    pinMode( SPOOF_SIGNAL_A, INPUT );
    pinMode( SPOOF_SIGNAL_B, INPUT );
    pinMode( SPOOF_ENGAGE, OUTPUT );

    // Initialize the DAC board by setting the DAC Chip Select
    digitalWrite( DAC_CS, HIGH );

    // Initialize relay board by clearing the Spoof Engage
    digitalWrite( SPOOF_ENGAGE, LOW );

    init_serial( );

    init_can( );

	pid_params.derivative_gain = current_ctrl_state.SA_Kd;
    pid_params.proportional_gain = current_ctrl_state.SA_Kp;
    pid_params.integral_gain = current_ctrl_state.SA_Ki;

    publish_ps_ctrl_steering_report( );

    current_ctrl_state.control_enabled = false;

    current_ctrl_state.emergency_stop = false;

    current_ctrl_state.override_flag.wheel = 0;

    current_ctrl_state.override_flag.voltage = 0;

    current_ctrl_state.override_flag.voltage_spike_a = 0;

    current_ctrl_state.override_flag.voltage_spike_b = 0;

    // Initialize the Rx timestamps to avoid timeout warnings on start up
    rx_frame_ps_ctrl_steering_command.timestamp = millis( );

    pid_zeroize( &pid_params, STEERING_WINDUP_GUARD );

    // debug log
    DEBUG_PRINT( "init: pass" );

    test_code = test_pid_values( pid_params, current_ctrl_state );
}


/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */


// *****************************************************
// Function:    loop
//
// Purpose:     Main processing loop for the steering control
//              The loop is called periodically and must check the elapsed time
//              to determine what to do
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void loop( )
{
	if( test_code == false )
	{
		exit( 0 );
	}
	else
	{
	    // checks for CAN frames, if yes, updates state variables
	    handle_ready_rx_frames( );

	    // publish all report CAN frames
	    publish_timed_tx_frames( );

	    // check all timeouts
	    check_rx_timeouts( );

	    uint32_t current_timestamp_us;

	    uint32_t deltaT = timer_delta_us( current_ctrl_state.timestamp_us,
	                                      &current_timestamp_us );

	    if ( deltaT > 50000 )
	    {

	        current_ctrl_state.timestamp_us = current_timestamp_us;

	        bool override = check_driver_steering_override( );

	        if ( override == true )
	        {
	            current_ctrl_state.override_flag.wheel = 1;
	            disable_control( );
	        }
	        else if ( current_ctrl_state.control_enabled == true )
	        {

	/*******************************************************************************
	*   WARNING
	*
	*   The ranges selected to do steering control are carefully tested to
	*   ensure that a torque is not requested that the vehicles steering motor
	*   cannot handle. By changing any of this code you risk attempting to actuate
	*   a torque outside of the vehicles valid range. Actuating a torque outside of
	*   the vehicles valid range will, at best, cause the vehicle to go into an
	*   unrecoverable fault state. Clearing this fault state requires one of Kia's
	*   native diagnostics tools, and someone who knows how to clear DTC codes with
	*   said tool.
	*
	*   It is NOT recommended to modify any of the existing control ranges, or
	*   gains, without expert knowledge.
	*******************************************************************************/

	            // Calculate steering angle rates (degrees/microsecond)
	            double steering_angle_rate =
	                ( current_ctrl_state.current_steering_angle -
	                  current_ctrl_state.steering_angle_last ) / 0.05;

	            double steering_angle_rate_target =
	                ( current_ctrl_state.commanded_steering_angle -
	                  current_ctrl_state.current_steering_angle ) / 0.05;

	            // Save the angle for next iteration
	            current_ctrl_state.steering_angle_last =
	                current_ctrl_state.current_steering_angle;

	            steering_angle_rate_target =
	                constrain( ( double )steering_angle_rate_target,
	                           ( double )-current_ctrl_state.steering_angle_rate_max,
	                           ( double )current_ctrl_state.steering_angle_rate_max );

	            pid_update(
	                    &pid_params,
	                    steering_angle_rate_target,
	                    steering_angle_rate,
	                    0.050 );

	            double control = pid_params.control;

	            control = constrain( ( float ) control,
	                                 ( float ) -1500.0f,
	                                 ( float ) 1500.0f );

	            struct torque_spoof_t torque_spoof;

	            calculate_torque_spoof( control, &torque_spoof );

	            torque_sum = (uint8_t) ( torque_spoof.low + torque_spoof.high );

	            do_dac_output( &torque_spoof );
	        }
	        else
	        {
	            current_ctrl_state.override_flag.wheel = 0;

	            pid_zeroize( &pid_params, STEERING_WINDUP_GUARD );
	        }
	    }
	}
}
