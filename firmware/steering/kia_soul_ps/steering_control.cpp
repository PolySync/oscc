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

// Source for steering control ECU firmware
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

#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT( x )  Serial.println( x )
    #define STATIC
#else
    #define DEBUG_PRINT( x )
    #define STATIC static
#endif

// *****************************************************
// Function:    timer_delta_ms
//
// Purpose:     Calculate the milliseconds between the current time and the
//              input and correct for the timer overflow condition
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] timestamp - the last time sample
//
// *****************************************************
uint32_t timer_delta_ms( uint32_t last_time )
{
    uint32_t delta = 0;
    uint32_t current_time = millis( );

    if ( current_time < last_time )
    {
        // Timer overflow
        delta = ( UINT32_MAX - last_time ) + current_time;
    }
    else
    {
        delta = current_time - last_time;
    }
    return ( delta );
}


// *****************************************************
// Function:    timer_delta_us
//
// Purpose:     Calculate the microseconds between the current time and the
//              input and correct for the timer overflow condition
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] last_sample - the last time sample
//              [in] current_sample - pointer to store the current time
//
// *****************************************************
uint32_t timer_delta_us( uint32_t last_time, uint32_t* current_time )
{
    uint32_t delta = 0;
    uint32_t local_time = micros( );

    if ( local_time < last_time )
    {
        // Timer overflow
        delta = ( UINT32_MAX - last_time ) + local_time;
    }
    else
    {
        delta = local_time - last_time;
    }

    if ( current_time != NULL )
    {
        *current_time = local_time;
    }

    return ( delta );
}


/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */

// *****************************************************
// Function:    average_samples
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over the indicated number of samples
//              Function takes 260us * num_samples to run
//
// Returns:     int16_t - SUCCESS or FAILURE
//
// Parameters:  [in]  num_samples - the number of samples to average
//              [out] averages - array of values to store the averages
//
// *****************************************************
int16_t average_samples( int16_t num_samples, int16_t* averages )
{
    int16_t return_code = FAILURE;

    if ( averages != NULL )
    {
        return_code = SUCCESS;

        int32_t sums[ 2 ] = { 0, 0 };

        for ( int16_t i = 0; i < num_samples; i++ )
        {
            sums[ SAMPLE_A ] += analogRead( SIGNAL_INPUT_A );
            sums[ SAMPLE_B ] += analogRead( SIGNAL_INPUT_B );
        }

        averages[ SAMPLE_A ] = ( sums[ SAMPLE_A ] / num_samples ) << 2;
        averages[ SAMPLE_B ] = ( sums[ SAMPLE_B ] / num_samples ) << 2;
    }
    return ( return_code );
}

// *****************************************************
// Function:    enable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  [in] dac - the dac structure
//				[in] current_ctrl_state - the control state structure
//
// *****************************************************
void enable_control( DAC_MCP49xx &dac, current_control_state &current_ctrl_state )
{
    static int16_t num_samples = 20;
    int16_t averages[ 2 ] = { 0, 0 };

    int16_t status = average_samples( num_samples, averages );

    if ( SUCCESS == status )
    {
        // Write measured torque values to DAC to avoid a signal
        // discontinuity when the SCM takes over
        dac.outputA( averages[ SAMPLE_A ] );
        dac.outputB( averages[ SAMPLE_B ] );

        // Enable the signal interrupt relays
        digitalWrite( SPOOF_ENGAGE, HIGH );

        current_ctrl_state.control_enabled = true;

        DEBUG_PRINT( "Control enabled" );
    }
}

// *****************************************************
// Function:    disable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  [in] dac - the dac structure
//				[in] current_ctrl_state - the control state structure
//
// *****************************************************
void disable_control( DAC_MCP49xx &dac, current_control_state &current_ctrl_state )
{
    if ( current_ctrl_state.control_enabled == true )
    {
        static int16_t num_samples = 20;
        int16_t averages[ 2 ] = { 0, 0 };

        average_samples( num_samples, averages );

        // Write measured torque values to DAC to avoid a signal
        // discontinuity when the SCM takes over
        dac.outputA( averages[ SAMPLE_A ] );
        dac.outputB( averages[ SAMPLE_B ] );
    }

    current_ctrl_state.control_enabled = false;

    // Disable the signal interrupt relays
    digitalWrite( SPOOF_ENGAGE, LOW );

    DEBUG_PRINT( "Control disabled" );
}


// *****************************************************
// Function:    check_driver_steering_override
//
// Purpose:     This function checks the voltage input from the steering
//              wheel's torque sensors to determine if the driver is attempting
//              to steer the vehicle.  This must be done over time by taking
//              periodic samples of the input torque voltage, calculating the
//              difference between the two and then passing that difference
//              through a basic exponential filter to smooth the input.
//
//              The required response time for the filter is 250 ms, which at
//              50ms per sample is 5 samples.  As such, the alpha for the
//              exponential filter is 0.5 to make the input go "close to" zero
//              in 5 samples.
//
//              The implementation is:
//                  s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )
//
//              If the filtered torque exceeds the max torque, it is an
//              indicator that there is feedback on the steering wheel and the
//              control should be disabled.
//
//              The final check determines if the a and b signals are opposite
//              each other.  If they are not, it is an indicator that there is
//              a problem with one of the sensors.  The check is looking for a
//              90% tolerance.
//
// Returns:     true if the driver is requesting an override
//
// Parameters:  None
//
// *****************************************************
bool check_driver_steering_override( )
{
    // The parameters below; torque_filter_alpha and steering_wheel_max_torque,
    // can be used to modify how selective the steering override functionality
    // is. If torque_filter_alpha or steering_wheel_max_torque is increased
    // then steering override will be more selective about disabling on driver
    // input. That is, it will require a harder input for the steering wheel
    // to automatically disable. If these values are lowered then the steering
    // override will be less selective; this may result in drastic movements
    // of the joystick controller triggering steering override.
    // It is expected behavior that if a user uses the joystick controller to
    // purposefully "fight" the direction of steering wheel movement that this
    // will cause a steering override with the below parameters. That is if
    // the steering wheel is drastically "jerked" back and forth, opposing the
    // direction of steering wheel movement and purposefully trying to cause
    // an unstable situation, the steering override is expected to be
    // triggered.
    static const float torque_filter_alpha = 0.5;
    static const float steering_wheel_max_torque = 3000.0;

    static float filtered_torque_a = 0.0;
    static float filtered_torque_b = 0.0;

    bool override = false;

    float torque_sensor_a = ( float )( analogRead( SIGNAL_INPUT_A ) << 2 );
    float torque_sensor_b = ( float )( analogRead( SIGNAL_INPUT_B ) << 2 );

    filtered_torque_a =
        ( torque_filter_alpha * torque_sensor_a ) +
            ( ( 1.0 - torque_filter_alpha ) * filtered_torque_a );

    filtered_torque_b =
        ( torque_filter_alpha * torque_sensor_b ) +
            ( ( 1.0 - torque_filter_alpha ) * filtered_torque_b );

    if ( ( abs( filtered_torque_a ) > steering_wheel_max_torque ) ||
         ( abs( filtered_torque_b ) > steering_wheel_max_torque ) )
    {
        override = true;
    }

    return ( override );
}


// *****************************************************
// Function:    calculate_torque_spoof
//
// Purpose:     Container for hand-tuned empirically determined values
//
//              Values calculated with min/max calibration curve and hand
//              tuned for neutral balance.
//              DAC requires 12-bit values = (4096steps/5V = 819.2 steps/V)
//
// Returns:     void
//
// Parameters:  [in] torque - floating point value with the current torque value
//              [out] torque_spoof - structure containing the integer torque values
//
// *****************************************************
void calculate_torque_spoof( float torque, struct torque_spoof_t* spoof )
{
    spoof->low = 819.2 * ( 0.0008 * torque + 2.26 );
    spoof->high = 819.2 * ( -0.0008 * torque + 2.5 );
}

/* ====================================== */
/* =========== COMMUNICATIONS =========== */
/* ====================================== */


// *****************************************************
// Function:    publish_ps_ctrl_steering_report
//
// Purpose:     Fill out the transmit CAN frame with the steering angle
//              and publish that information on the CAN bus
//
// Returns:     void
//
// Parameters:  [in] tx_frame_ps_ctrl_steering_report
//				[in] current_ctrl_state
//				[in] torque_sum
//				[in] CAN
//
// *****************************************************
void publish_ps_ctrl_steering_report( can_frame_s &tx_frame_ps_ctrl_steering_report,
                                    current_control_state &current_ctrl_state,
                                    uint8_t &torque_sum,
                                    MCP_CAN &CAN )
{
    tx_frame_ps_ctrl_steering_report.id =
        ( uint32_t ) ( PS_CTRL_MSG_ID_STEERING_REPORT );

    tx_frame_ps_ctrl_steering_report.dlc = 8;

    // Get a pointer to the data buffer in the CAN frame and set
    // the steering angle
    ps_ctrl_steering_report_msg * data =
        ( ps_ctrl_steering_report_msg* ) tx_frame_ps_ctrl_steering_report.data;

    data->angle = current_ctrl_state.current_steering_angle;

    tx_frame_ps_ctrl_steering_report.timestamp = millis( );

    // set override flag
    if ( ( current_ctrl_state.override_flag.wheel == 0 ) &&
            ( current_ctrl_state.override_flag.voltage == 0 ) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->angle_command = current_ctrl_state.commanded_steering_angle;

    data->torque = torque_sum;

    data->enabled = (uint8_t) current_ctrl_state.control_enabled;

    CAN.sendMsgBuf( tx_frame_ps_ctrl_steering_report.id,
                    0,
                    tx_frame_ps_ctrl_steering_report.dlc,
                    tx_frame_ps_ctrl_steering_report.data );
}


// *****************************************************
// Function:    publish_timed_tx_frames
//
// Purpose:     Determine if enough time has passed to publish the steering
//              report to the CAN bus again
//
// Returns:     void
//
// Parameters:  [in] tx_frame_ps_ctrl_steering_report
//				[in] current_ctrl_state
//				[in] torque_sum
//				[in] CAN
//
// *****************************************************
void publish_timed_tx_frames( can_frame_s &tx_frame_ps_ctrl_steering_report,
                            current_control_state &current_ctrl_state,
                            uint8_t &torque_sum,
                            MCP_CAN &CAN )
{
    uint32_t delta =
        timer_delta_ms( tx_frame_ps_ctrl_steering_report.timestamp );

    if ( delta >= PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL )
    {
        publish_ps_ctrl_steering_report( tx_frame_ps_ctrl_steering_report,
                                        current_ctrl_state,
                                        torque_sum,
                                        CAN );
    }
}


// *****************************************************
// Function:    process_ps_ctrl_steering_command
//
// Purpose:     Process a steering command message
//
// Returns:     void
//
// Parameters:  [in] control_data -  pointer to a steering command control message
//				[in] dac - DAC structure
//				[in] current_ctrl_state - current control state structure
//				[in] rx_frame_ps_ctrl_steering_command - steering command
//
// *****************************************************
void process_ps_ctrl_steering_command(
    const ps_ctrl_steering_command_msg * const control_data,
            DAC_MCP49xx &dac,
            current_control_state &current_ctrl_state,
            can_frame_s &rx_frame_ps_ctrl_steering_command )
{
    current_ctrl_state.commanded_steering_angle =
        control_data->steering_wheel_angle_command / 9.0;

    current_ctrl_state.steering_angle_rate_max =
        control_data->steering_wheel_max_velocity * 9.0;

    if ( ( control_data->enabled == 1 ) &&
            ( current_ctrl_state.control_enabled == false ) &&
            ( current_ctrl_state.emergency_stop == false ) )
    {
        current_ctrl_state.control_enabled = true;
        enable_control( dac, current_ctrl_state );
    }

    if ( ( control_data->enabled == 0 ) &&
            ( current_ctrl_state.control_enabled == true ) )
    {
        current_ctrl_state.control_enabled = false;
        disable_control( dac, current_ctrl_state );
    }

    rx_frame_ps_ctrl_steering_command.timestamp = millis( );
}


// *****************************************************
// Function:    process_psvc_chassis_state1
//
// Purpose:     Process the chassis state message
//
// Returns:     void
//
// Parameters:  [in] chassis_data - pointer to a chassis state message that contains
//                             		the steering angle
//				[in] current_ctrl_state - the current control state structure
//
// *****************************************************
void process_psvc_chassis_state1(
    const psvc_chassis_state1_data_s * const chassis_data,
            current_control_state &current_ctrl_state )
{
    float raw_angle = (float)chassis_data->steering_wheel_angle;
    current_ctrl_state.current_steering_angle = raw_angle * 0.0076294;

    // Convert from 40 degree range to 470 degree range in 1 degree increments
    current_ctrl_state.current_steering_angle *= 11.7;
}


// *****************************************************
// Function:    handle_ready_rx_frames
//
// Purpose:     Parse received CAN data and redirect to correct
//              processing function
//
// Returns:     void
//
// Parameters:  [in] dac - the dac structure
//				[in] current_ctrl_state - current control state structure
//				[in] CAN - CAN structure
//				[in] rx_frame_ps_ctrl_steering_command - steering command
//
// *****************************************************
void handle_ready_rx_frames( DAC_MCP49xx &dac,
            current_control_state &current_ctrl_state,
            MCP_CAN &CAN,
            can_frame_s &rx_frame_ps_ctrl_steering_command )
{
    if ( CAN.checkReceive() == CAN_MSGAVAIL )
    {
        can_frame_s rx_frame;

        memset( &rx_frame, 0, sizeof(rx_frame) );

        CAN.readMsgBufID( (INT32U*) &rx_frame.id,
                          (INT8U*) &rx_frame.dlc,
                          (INT8U*) rx_frame.data );

        if ( rx_frame.id == PS_CTRL_MSG_ID_STEERING_COMMAND )
        {
            process_ps_ctrl_steering_command(
                ( const ps_ctrl_steering_command_msg * const )rx_frame.data,
                    dac,
                    current_ctrl_state,
                    rx_frame_ps_ctrl_steering_command );
        }

        if ( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            process_psvc_chassis_state1(
                ( const psvc_chassis_state1_data_s * const )rx_frame.data,
                current_ctrl_state );
        }
    }
}


// *****************************************************
// Function:    check_rx_timeouts
//
// Purpose:     If the control is currently enabled, but the receiver indicates
//              a "watchdog" timeout, then disable the control
//
// Returns:     void
//
// Parameters:  [in] dac - the dac structure
//				[in] current_ctrl_state - current control state structure
//				[in] rx_frame_ps_ctrl_steering_command - steering command
//
// *****************************************************
void check_rx_timeouts( DAC_MCP49xx &dac,
						current_control_state &current_ctrl_state,
                        can_frame_s &rx_frame_ps_ctrl_steering_command )
{
    uint32_t delta =
        timer_delta_ms( rx_frame_ps_ctrl_steering_command.timestamp );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        DEBUG_PRINT( "Control disabled: Timeout" );
        disable_control( dac, current_ctrl_state );
    }
}

