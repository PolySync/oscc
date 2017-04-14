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
#include "PID.h"
#include "DAC_MCP49xx.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "control.h"
#include "debug.h"

#include "steering_params.h"
#include "steering_state.h"




// *****************************************************
// static global types/macros
// *****************************************************


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
static steering_state_s steering_state;


//
static control_state_s control_state;


//
static kia_soul_steering_params_s steering_params;


//
static PID pid;


//
static uint8_t torque_sum;


// *****************************************************
// static declarations
// *****************************************************




/* ====================================== */
/* ============== CONTROL =============== */
/* ====================================== */


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

    data->angle = steering_state.steering_angle;

    tx_frame_ps_ctrl_steering_report.timestamp = millis( );

    // set override flag
    if ( ( steering_state.override_flags.wheel == 0 ) &&
            ( steering_state.override_flags.voltage == 0 ) )
    {
        data->override = 0;
    }
    else
    {
        data->override = 1;
    }

    data->angle_command = steering_state.steering_angle_target;

    data->torque = torque_sum;

    data->enabled = (uint8_t) control_state.enabled;

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
        timer_delta_ms( tx_frame_ps_ctrl_steering_report.timestamp, NULL );

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
    steering_state.steering_angle_target =
        control_data->steering_wheel_angle_command / 9.0;

    steering_params.steering_angle_rate_max =
        control_data->steering_wheel_max_velocity * 9.0;

    if ( ( control_data->enabled == 1 ) &&
            ( control_state.enabled == false ) &&
            ( control_state.emergency_stop == false ) )
    {
        enable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
    }

    if ( ( control_data->enabled == 0 ) &&
            ( control_state.enabled == true ) )
    {
        disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
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
    steering_state.steering_angle = raw_angle * 0.0076294;

    // Convert from 40 degree range to 470 degree range in 1 degree increments
    steering_state.steering_angle *= 11.7;
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
        timer_delta_ms( rx_frame_ps_ctrl_steering_command.timestamp, NULL );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        DEBUG_PRINTLN( "Control disabled: Timeout" );
        disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
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

    #ifdef DEBUG
        init_serial( );
    #endif

    init_can(CAN);

    publish_ps_ctrl_steering_report( );

    control_state.enabled = false;

    control_state.emergency_stop = false;

    steering_state.override_flags.wheel = 0;

    steering_state.override_flags.voltage = 0;

    steering_state.override_flags.voltage_spike_a = 0;

    steering_state.override_flags.voltage_spike_b = 0;

    // Initialize the Rx timestamps to avoid timeout warnings on start up
    rx_frame_ps_ctrl_steering_command.timestamp = millis( );

    pid_zeroize( &pid, STEERING_WINDUP_GUARD );

    // debug log
    DEBUG_PRINTLN( "init: pass" );
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

    uint32_t deltaT = timer_delta_us( control_state.timestamp_us,
                                      &current_timestamp_us );

    if ( deltaT > 50000 )
    {

        control_state.timestamp_us = current_timestamp_us;

        bool override = check_driver_steering_override( );

        if ( override == true )
        {
            steering_state.override_flags.wheel = 1;
            disable_control( SIGNAL_INPUT_A, SIGNAL_INPUT_B, SPOOF_ENGAGE, &control_state, &dac );
        }
        else if ( control_state.enabled == true )
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
                ( steering_state.steering_angle -
                  steering_state.steering_angle_last ) / 0.05;

            double steering_angle_rate_target =
                ( steering_state.steering_angle_target -
                  steering_state.steering_angle ) / 0.05;

            // Save the angle for next iteration
            steering_state.steering_angle_last =
                steering_state.steering_angle;

            steering_angle_rate_target =
                constrain( ( double )steering_angle_rate_target,
                           ( double )-steering_params.steering_angle_rate_max,
                           ( double )steering_params.steering_angle_rate_max );

            pid.derivative_gain = steering_params.SA_Kd;
            pid.proportional_gain = steering_params.SA_Kp;
            pid.integral_gain = steering_params.SA_Ki;

            pid_update(
                    &pid,
                    steering_angle_rate_target,
                    steering_angle_rate,
                    0.050 );

            double control = pid.control;

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
            steering_state.override_flags.wheel = 0;

            pid_zeroize( &pid, STEERING_WINDUP_GUARD );
        }
    }
}
