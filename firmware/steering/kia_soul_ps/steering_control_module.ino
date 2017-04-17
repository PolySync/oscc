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




// *****************************************************
// static global types/macros
// *****************************************************

#define PSYNC_DEBUG_FLAG ( true )

#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT( x )  Serial.println( x )
#else
    #define DEBUG_PRINT( x )
#endif

// set CAN_CS to pin 10 for CAN
#define CAN_CS                          ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT         ( 250 )

// set up pins for interface with DAC (MCP4922)
#define DAC_CS                          ( 9 )       // Chip select pin

// Windup guard for steering PID controller
#define STEERING_WINDUP_GUARD           ( 1500 )

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
#define VOLTAGE_THRESHOLD               ( 0.096 )     // mV

#define SAMPLE_A                        ( 0 )

#define SAMPLE_B                        ( 1 )

#define FAILURE                         ( 0 )

#define SUCCESS                         ( 1 )


// *****************************************************
// local defined data structures
// *****************************************************

struct torque_spoof_t
{
    uint16_t low;
    uint16_t high;
};


// *****************************************************
// static structures
// *****************************************************


DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, 9 );     // DAC model, SS pin, LDAC pin

// Construct the CAN shield object
MCP_CAN CAN( CAN_CS );                          // Set CS pin for the CAN shield


//
static can_frame_s rx_frame_ps_ctrl_steering_command;


//
static can_frame_s tx_frame_ps_ctrl_steering_report;


//
static current_control_state current_ctrl_state;


//
static PID pid_params;


//
static uint8_t torque_sum;


// *****************************************************
// static declarations
// *****************************************************


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
static uint32_t timer_delta_ms( uint32_t last_time )
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
static uint32_t timer_delta_us( uint32_t last_time, uint32_t* current_time )
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


// *****************************************************
// Function:    init_serial
//
// Purpose:     Initializes the serial port communication
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
static void init_serial( )
{
    Serial.begin( SERIAL_BAUD );

    DEBUG_PRINT( "init_serial: pass" );
}


// *****************************************************
// Function:    init_can
//
// Purpose:     Initializes the CAN communication
//              Function must iterate while the CAN module initializes
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
static void init_can ( void )
{
    while ( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        DEBUG_PRINT( "init_can: retrying" );

        delay( CAN_INIT_RETRY_DELAY );
    }

    DEBUG_PRINT( "init_can: pass" );
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
static int16_t average_samples( int16_t num_samples, int16_t* averages )
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
// Parameters:  None
//
// *****************************************************
void enable_control( )
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
// Parameters:  None
//
// *****************************************************
void disable_control( )
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
// Parameters:  None
//
// *****************************************************
static void publish_ps_ctrl_steering_report( )
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
// Parameters:  None
//
// *****************************************************
static void publish_timed_tx_frames( )
{
    uint32_t delta =
        timer_delta_ms( tx_frame_ps_ctrl_steering_report.timestamp );

    if ( delta >= PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL )
    {
        publish_ps_ctrl_steering_report();
    }
}


// *****************************************************
// Function:    process_ps_ctrl_steering_command
//
// Purpose:     Process a steering command message
//
// Returns:     void
//
// Parameters:  control_data -  pointer to a steering command control message
//
// *****************************************************
static void process_ps_ctrl_steering_command(
    const ps_ctrl_steering_command_msg * const control_data )
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
        enable_control( );
    }

    if ( ( control_data->enabled == 0 ) &&
            ( current_ctrl_state.control_enabled == true ) )
    {
        current_ctrl_state.control_enabled = false;
        disable_control( );
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
// Parameters:  chassis_data - pointer to a chassis state message that contains
//                             the steering angle
//
// *****************************************************
static void process_psvc_chassis_state1(
    const psvc_chassis_state1_data_s * const chassis_data )
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
// Parameters:  None
//
// *****************************************************
void handle_ready_rx_frames( )
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
                ( const ps_ctrl_steering_command_msg * const )rx_frame.data );
        }

        if ( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            process_psvc_chassis_state1(
                ( const psvc_chassis_state1_data_s * const )rx_frame.data );
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
// Parameters:  None
//
// *****************************************************
static void check_rx_timeouts( )
{
    uint32_t delta =
        timer_delta_ms( rx_frame_ps_ctrl_steering_command.timestamp );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        DEBUG_PRINT( "Control disabled: Timeout" );
        disable_control();
    }
}



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

            pid_params.derivative_gain = current_ctrl_state.SA_Kd;
            pid_params.proportional_gain = current_ctrl_state.SA_Kp;
            pid_params.integral_gain = current_ctrl_state.SA_Ki;

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

            dac.outputA( torque_spoof.low );
            dac.outputB( torque_spoof.high );
        }
        else
        {
            current_ctrl_state.override_flag.wheel = 0;

            pid_zeroize( &pid_params, STEERING_WINDUP_GUARD );
        }
    }
}
