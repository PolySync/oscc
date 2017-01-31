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
// J Hartung, 2015


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

// Set CAN_CS to pin 10 for CAN
#define CAN_CS      ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT ( 200 )
#define STEERING_LOOP_TIME_MS ( 50 )

// Set up pins for interface with the DAC (MCP4922)

#define DAC_CS                ( 9 )     // Chip select pin

#define SIGNAL_INPUT_A        ( A0 )    // Sensing input for the DAC output

#define SIGNAL_INPUT_B        ( A1 )    // Green wire from the torque sensor, low values

#define SPOOF_SIGNAL_A        ( A2 )    // Sensing input for the DAC output

#define SPOOF_SIGNAL_B        ( A3 )    // Blue wire from the torque sensor, high values

#define SPOOF_ENGAGE          ( 6 )     // Signal interrupt (relay) for spoofed torque values

#define SAMPLE_A    ( 0 )
#define SAMPLE_B    ( 1 )

#define FAILURE     ( 0 )
#define SUCCESS     ( 1 )

// *****************************************************
// local defined data structures
// *****************************************************

struct torque_spoof_s
{
    uint16_t low;
    uint16_t high;
};

struct timer_struct_s
{
    uint32_t timestamp;
    const uint32_t period;
};


// *****************************************************
// static structures
// *****************************************************

static int8_t driver_override = 0;

DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, 9 );     // DAC model, SS pin, LDAC pin

// Construct the CAN shield object
MCP_CAN CAN( CAN_CS );                            // Set CS pin for the CAN shield


//
static struct timer_struct_s rx_steering_command_timestamp =
    { 0, PS_CTRL_RX_WARN_TIMEOUT };


//
static struct timer_struct_s tx_steering_report_timestamp =
    { 0, PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL };


//
static current_control_state current_ctrl_state;
static struct timer_struct_s ctrl_state_timestamp =
    { 0, STEERING_LOOP_TIME_MS };

//
static PID pid_params;


// *****************************************************
// static declarations
// *****************************************************


// *****************************************************
// Function:    schedule_timer
// 
// Purpose:     Set a timer expiration at some point in the future based on the
//              current time
// 
// Returns:     void
// 
// Parameters:  [in/out] timer - pointer to the timer structure to schedule
// 
// *****************************************************
static void schedule_timer( struct timer_struct_s* const timer )
{
    if ( timer != NULL )
    {
        timer->timestamp = millis();
        timer->timestamp += timer->period;
    }
}


// *****************************************************
// Function:    update_periodic_timer
// 
// Purpose:     Update the timer to the next period for this timer
// 
// Returns:     void
// 
// Parameters:  [in/out] timer - the timer structure to update
// 
// *****************************************************
static void update_periodic_timer( struct timer_struct_s* const timer )
{
    if ( timer != NULL )
    {
        timer->timestamp += timer->period;
    }
}


// *****************************************************
// Function:    is_timer_expired
// 
// Purpose:     Determine if the current time is greater than the input
// 
// Returns:     bool - the timer has expired
// 
// Parameters:  [in/out] timer - the time to check current against
// 
// *****************************************************
static bool is_timer_expired( const struct timer_struct_s* const timer )
{
    bool expired = true;

    if ( timer != NULL )
    {
        uint32_t current_time = millis( );

        if ( timer->timestamp > current_time )
        {
            expired = false;
        }
    }
    return ( expired );
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
void disable_control() 
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
    digitalWrite(SPOOF_ENGAGE, LOW);

    DEBUG_PRINT("Control disabled");
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
    static const float torque_filter_alpha = 0.5;
    static const float steering_wheel_max_torque = 2500.0;

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
    else
    {
        float difference = abs( torque_sensor_a + torque_sensor_b );

        if ( difference > ( steering_wheel_max_torque / 10.0 ) )
        {
            override = true;
        }
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
void calculate_torque_spoof( float torque, struct torque_spoof_s* spoof )
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
    can_frame_s tx_frame;

    tx_frame.id = ( uint32_t )( PS_CTRL_MSG_ID_STEERING_REPORT );

    tx_frame.dlc = 8;

    // Get a pointer to the data buffer in the CAN frame and set
    // the steering angle
    ps_ctrl_steering_report_msg * data =
        ( ps_ctrl_steering_report_msg* )tx_frame.data;

    data->angle = current_ctrl_state.current_steering_angle;

    data->override = driver_override;
    
    CAN.sendMsgBuf( tx_frame.id, 0, tx_frame.dlc, tx_frame.data );
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
    bool timer_expired = is_timer_expired( &tx_steering_report_timestamp );

    if ( timer_expired == true )
    {
        update_periodic_timer( &tx_steering_report_timestamp );

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
        enable_control( );
    }

    if ( ( control_data->enabled == 0 ) &&
         ( current_ctrl_state.control_enabled == true ) )
    {
        disable_control();
    }

    schedule_timer( &rx_steering_command_timestamp );
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
    bool timer_expired = is_timer_expired( &rx_steering_command_timestamp );

    if ( timer_expired == true ) 
    {
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

    schedule_timer( &ctrl_state_timestamp );

    // Initialize the Rx and Tx timestamps to avoid timeout warnings on start up
    schedule_timer( &rx_steering_command_timestamp );

    schedule_timer( &tx_steering_report_timestamp );

    pid_zeroize( &pid_params );

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
    handle_ready_rx_frames( );
      
    publish_timed_tx_frames( );

    check_rx_timeouts( );

    bool timer_expired = is_timer_expired( &ctrl_state_timestamp );

    if ( timer_expired == true )
    {
        update_periodic_timer( &ctrl_state_timestamp );

        bool override = check_driver_steering_override( );

        if ( override == true )
        {
            driver_override = 1;
            disable_control( );
        }
        else if ( current_ctrl_state.control_enabled == true ) 
        {
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
            
            double steering_angle_rate_error =
                steering_angle_rate_target - steering_angle_rate;

            pid_params.derivative_gain = current_ctrl_state.SA_Kd;
            pid_params.proportional_gain = current_ctrl_state.SA_Kp;
            pid_params.integral_gain = current_ctrl_state.SA_Ki;

            pid_update( &pid_params, steering_angle_rate_error, 0.050 );

            double control = pid_params.control;

            control = constrain( ( float ) control,
                                 ( float ) -1500.0f,
                                 ( float ) 1500.0f );

            struct torque_spoof_s torque_spoof;

            calculate_torque_spoof( control, &torque_spoof );

            dac.outputA( torque_spoof.high );
            dac.outputB( torque_spoof.low );
        }
        else
        {
            pid_zeroize( &pid_params );
        }
    }
}

