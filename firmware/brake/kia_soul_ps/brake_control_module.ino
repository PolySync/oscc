/************************************************************************/
/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */
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

#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"
#include "control_protocol_can.h"
#include "PID.h"


// *****************************************************
// Debug enable/disable
// *****************************************************

#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
    #warning "PSYNC_DEBUG_FLAG defined"
    #define DEBUG_PRINT(X)  Serial.println(X)
#else
    #define DEBUG_PRINT(X)
#endif


// *****************************************************
// Constants
// *****************************************************

// All pressures are in tenths of a bar (decibars) to match
// the values coming from the vehicle
//const float MIN_ACCUMULATOR_PRESSURE = 2.1;              // min accumulator pressure to maintain
const float MIN_ACCUMULATOR_PRESSURE = 777.6;   // min pressure to maintain (decibars)
//const float MAX_ACCUMULATOR_PRESSURE = 2.3;               // max accumulator pressure to maintain (bar)
const float MAX_ACCUMULATOR_PRESSURE = 878.3;   // max pressure to maintain (decibars)
//const float PEDAL_THRESHOLD = 0.6;       // Pressure for pedal interference
const float PEDAL_THRESHOLD = 43.2;             // Pedal pressure for driver override

// *****************************************************
// static global data/macros
// *****************************************************

#define CAN_SHIELD_CHIP_SELECT_PIN (53)

// ms
#define PS_CTRL_RX_WARN_TIMEOUT (150)
#define BRAKE_CONTROL_LOOP_INTERVAL (10)

#define BRAKE_PID_WINDUP_GUARD (500)

// The min/max duty cycle scalars used for 3.921 KHz PWM frequency.
// These represent the minimum duty cycles that begin to actuate the
// proportional solenoids and the maximum duty cycle where the solenoids
// have reached their stops.
#define SLA_DUTY_CYCLE_MAX (105.0)
#define SLA_DUTY_CYCLE_MIN (50.0)
#define SLR_DUTY_CYCLE_MAX (100.0)
#define SLR_DUTY_CYCLE_MIN (50.0)

// ******************************************************************
// MOSFET pin (digital) definitions ( MOSFETs control the solenoids )
// ******************************************************************

// Pins are not perfectly sequential because the clock frequency of certain
// pins are different

// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)

const uint8_t PIN_SLAFL = 5;        // front left accumulator solenoid
const uint8_t PIN_SLAFR = 7;        // front right accumulator solenoid

// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)

const uint8_t PIN_SLRFL = 6;        // front left release solenoid
const uint8_t PIN_SLRFR = 8;        // front right release solenoid
const uint8_t PIN_SMC   = 2;        // master cylinder solenoids

// Digital pin controls

const uint8_t PIN_BRAKE_LIGHT = 48; // Tail light control
const uint8_t PIN_ACCUMULATOR_PUMP  = 49;   // accumulator pump motor

// Analog sensor pin definitions

const uint8_t PIN_PACC = 9;         // accumulator pressure sensor
const uint8_t PIN_PMC1 = 10;        // master cylinder pressure sensor 1
const uint8_t PIN_PMC2 = 11;        // master cylinder pressure sensor 2
const uint8_t PIN_PFR  = 13;        // front right pressure sensor
const uint8_t PIN_PFL  = 14;        // front left pressure sensor


// *****************************************************
// static structures
// *****************************************************

struct accumulator_data_s
{
    float pressure;
};

// master solenoid structure
struct master_cylinder_data_s
{
    float pressure1;
    float pressure2;
};

struct interpolate_range_s
{
    float input_min;
    float input_max;
    float output_min;
    float output_max;
};

// brake structure
struct brake_data_s
{
    float pressure;

    bool enable;
    bool enable_request;

    uint32_t rx_timestamp;

    int16_t driver_override;

    int16_t can_pressure;
    uint16_t pedal_command;
};


// *****************************************************
// static global declarations
// *****************************************************

// construct the CAN shield object
MCP_CAN CAN(CAN_SHIELD_CHIP_SELECT_PIN);  // Set CS pin for the CAN shield

PID pid_params;


// *****************************************************
// Accumulator data values
// *****************************************************

struct accumulator_data_s accumulator =
{
    0.0     // pressure
};

// *****************************************************
// Master Cyliner data values
// *****************************************************

struct master_cylinder_data_s master_cylinder =
{
    0.0,    // pressure1
    0.0     // pressure2
};

// *****************************************************
// Brake controller values
// *****************************************************

struct brake_data_s brakes =
{
    0.0,                        // pressure
    false,                      // enable
    false,                      // enable_request
    0,                          // rx_timestamp
    0,                          // driver_override
    0,                          // can_pressure
    0                           // pedal_command
};


// *****************************************************
// Helper functions
// *****************************************************


// *****************************************************
// Function:    init_serial
// 
// Purpose:     Initializes the serial port communication
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void init_serial( void )
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
// Parameters:  void
// 
// *****************************************************
void init_can( void )
{
    while ( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINT( "init_can: retrying" );
    }

    DEBUG_PRINT( "init_can: pass" );
}


// *****************************************************
// Function:    interpolate
//
// Purpose:     Perform a linear interpolation
//              This functions specifically performs a linear interpolation of
//              form y = mx + b.
//              1) Normalize the input in the range from 0 to 1
//              2) Scale the output over the range defined by the output min
//                 and max values
//              3) Translate the final result into the output range
//
// Returns:     float output
//
// Parameters:  input - value in the input range
//              range - structure that defines the input and output ranges
//
// *****************************************************
float interpolate( float input, struct interpolate_range_s* range )
{
    float output = input;

    if ( range != NULL )
    {
        output = ( input - range->input_min );
        output /= ( range->input_max - range->input_min );
        output *= ( range->output_max - range->output_min );
        output += range->output_min;
    }
    return ( output );
}

// *****************************************************
// Function:    timer_delta_ms
// 
// Purpose:     Calculate the milliseconds between the current time and the
//              input and correct for the timer overflow condition
// 
// Returns:     uint32_t the time delta between the two inputs
// 
// Parameters:  last_time - the last time sample
// 
// *****************************************************
uint32_t timer_delta_ms( uint32_t last_time )
{
    uint32_t delta = 0;
    uint32_t current_time = millis( );

    if ( current_time < last_time )
    {
        delta = ( UINT32_MAX - last_time ) + current_time;
    }   
    else
    {   
        delta = current_time - last_time;
    }
    return ( delta );
}


// *****************************************************
// Function:    raw_adc_to_pressure
// 
// Purpose:     Convert the raw ADC reading (0 - 1023)
//              to a pressure between 1.2 - 90.0 bar
//
//              Pressures are measured in tenths of a bar (decibars)
//              to match the values used on the vehicle; the range is
//              actually 12.0 - 900.0 decibars
//
//              pressure = m( raw adc ) + b
//
//              Empirically determined:
//              m = 2.4
//              b = -252.1
//
//              pressure = 2.4 * ( raw adc bits ) - 252.1
// 
// Returns:     float - pressure
// 
// Parameters:  input - raw ADC reading 
// 
// *****************************************************
float raw_adc_to_pressure( uint16_t input )
{
    float pressure = ( float )input;
    pressure *= 2.4;
    pressure -= 252.1;

    return ( pressure );
}


// *****************************************************
// Accumulator functions
// *****************************************************

// *****************************************************
// Function:    accumulator_turn_pump_off
// 
// Purpose:     Turn off accumulator pump
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void accumulator_turn_pump_off( )
{
    digitalWrite( PIN_ACCUMULATOR_PUMP, LOW );
}

// *****************************************************
// Function:    accumulator_turn_pump_on
// 
// Purpose:     Turn on accumulator pump
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void accumulator_turn_pump_on( )
{
    digitalWrite( PIN_ACCUMULATOR_PUMP, HIGH );
}

// *****************************************************
// Function:    accumulator_maintain_pressure
// 
// Purpose:     Turn accumulator pump on or off
//              to maintain pressure
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void accumulator_maintain_pressure( )
{
    const float accumulator_alpha = 0.05;

    uint16_t raw_accumulator_data = analogRead( PIN_PACC );

    float pressure = raw_adc_to_pressure( raw_accumulator_data );

    accumulator.pressure =
        ( accumulator_alpha * pressure ) +
        ( ( 1.0 - accumulator_alpha ) * accumulator.pressure );

    if ( accumulator.pressure < MIN_ACCUMULATOR_PRESSURE )
    {
        accumulator_turn_pump_on( );
    }

    if ( accumulator.pressure > MAX_ACCUMULATOR_PRESSURE )
    {
        accumulator_turn_pump_off( );
    }
}

// *****************************************************
// Function:    accumulator_init
// 
// Purpose:     Initializes the accumulator
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void accumulator_init( )
{
    pinMode( PIN_ACCUMULATOR_PUMP, OUTPUT );

    accumulator_turn_pump_off( );
}


// *****************************************************
// Master Cylinder functions
// *****************************************************


// *****************************************************
// Function:    master_cylinder_open
// 
// Purpose:     Open master cylinder solenoid
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void master_cylinder_open( )
{
    analogWrite( PIN_SMC, 0 );
}

// *****************************************************
// Function:    master_cylinder_close
// 
// Purpose:     Close master cylinder solenoid
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void master_cylinder_close( )
{
    analogWrite( PIN_SMC, 255 );
}


// *****************************************************
// Function:    master_cylinder_init
//
// Purpose:     Initializes the master cylinder solenoid
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void master_cylinder_init( )
{
    pinMode( PIN_SMC, OUTPUT );

    master_cylinder_open( );
}


// *****************************************************
// Brake controller functions
// *****************************************************


// *****************************************************
// Function:    brake_lights_off
// 
// Purpose:     Turn the brake lights off
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_lights_off( )
{
    digitalWrite( PIN_BRAKE_LIGHT, LOW );
}

// *****************************************************
// Function:    brake_lights_on
// 
// Purpose:     Turn the brake lights on
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_lights_on( )
{
    digitalWrite( PIN_BRAKE_LIGHT, HIGH );
}


// *****************************************************
// Function:    brake_command_actuator_solenoids
// 
// Purpose:     Sets the PWM that controls the "actuator" solenoids to the
//              the specified value
// 
// Returns:     void
// 
// Parameters:  uint16_t - duty_cycle - value to send to the PWM
// 
// *****************************************************
void brake_command_actuator_solenoids( uint16_t duty_cycle )
{
    analogWrite( PIN_SLAFL, duty_cycle );
    analogWrite( PIN_SLAFR, duty_cycle );
}


// *****************************************************
// Function:    brake_command_release_solenoids
// 
// Purpose:     Sets the PWM that controls the "release" solenoids to the
//              the specified value
// 
// Returns:     void
// 
// Parameters:  uint16_t - duty_cycle - value to send to the PWM
// 
// *****************************************************
void brake_command_release_solenoids( uint16_t duty_cycle )
{
    analogWrite( PIN_SLRFL, duty_cycle );
    analogWrite( PIN_SLRFR, duty_cycle );
}


// *****************************************************
// Function:    brake_enable
// 
// Purpose:     Enable brakes
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_enable( )
{
    if ( brakes.enable == false )
    {
        master_cylinder_close( );
        brake_command_release_solenoids( 0 );
        brakes.enable = true;
    }
}


// *****************************************************
// Function:    brake_disable
// 
// Purpose:     Disable the brakes
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_disable( )
{
    if ( brakes.enable == true )
    {
        brake_command_actuator_solenoids( 0 );

        brake_command_release_solenoids( 255 );

        brake_lights_off( );
        delay( 15 );

        master_cylinder_open( );

        brake_command_release_solenoids( 0 );

        brakes.enable = false;
    }
}


// *****************************************************
// Function:    brake_check_driver_override
//
// Purpose:     This function checks the voltage input from the brake pedal
//              sensors to determine if the driver is attempting to brake
//              the vehicle.  This must be done over time by taking
//              periodic samples of the input voltage, calculating the
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
//              If the filtered input exceeds the max voltage, it is an
//              indicator that the driver is pressing on the brake pedal
//              and the control should be disabled.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void brake_check_driver_override( )
{
    static const float filter_alpha = 0.05;

    static float filtered_input_1 = 0.0;
    static float filtered_input_2 = 0.0;

    float sensor_1 = ( float )( analogRead( PIN_PMC1 ) );
    float sensor_2 = ( float )( analogRead( PIN_PMC2 ) );

    sensor_1 = raw_adc_to_pressure( ( uint16_t )sensor_1 );
    sensor_2 = raw_adc_to_pressure( ( uint16_t )sensor_2 );

    filtered_input_1 = ( filter_alpha * sensor_1 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_1 );

    filtered_input_2 = ( filter_alpha * sensor_2 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_2 );

    if ( ( filtered_input_1 > PEDAL_THRESHOLD ) ||
         ( filtered_input_2 > PEDAL_THRESHOLD ) )
    {
        brakes.driver_override = 1;
        brakes.enable_request = false;
    }
    else
    {
        brakes.driver_override = 0;
    }
}


// *****************************************************
// Function:    brake_update_pressure
// 
// Purpose:     Update brake pressure
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_update_pressure( )
{
    uint16_t raw_left_pressure = analogRead( PIN_PFL );
    uint16_t raw_right_pressure = analogRead( PIN_PFR );

    float pressure_left = raw_adc_to_pressure( raw_left_pressure );
    float pressure_right = raw_adc_to_pressure( raw_right_pressure );

    brakes.pressure = ( pressure_left + pressure_right ) / 2;
}

// *****************************************************
// Function:    brake_init
// 
// Purpose:     Initializes the brakes
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_init( )
{
    // initialize solenoid pins to off
    digitalWrite( PIN_SLAFL, LOW );
    digitalWrite( PIN_SLAFR, LOW );
    digitalWrite( PIN_SLRFL, LOW );
    digitalWrite( PIN_SLRFR, LOW );

    // set pinmode to OUTPUT
    pinMode( PIN_SLAFL, OUTPUT );
    pinMode( PIN_SLAFR, OUTPUT );
    pinMode( PIN_SLRFL, OUTPUT );
    pinMode( PIN_SLRFR, OUTPUT );

    brake_lights_off( );
    pinMode( PIN_BRAKE_LIGHT, OUTPUT );
}

// *****************************************************
// Function:    publish_ps_ctrl_brake_report
// 
// Purpose:     Fill out the transmit CAN frame with the brake report
//              and publish that information on the CAN bus
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void publish_ps_ctrl_brake_report( )
{
    ps_ctrl_brake_report_msg report;

    report.override = ( uint8_t )brakes.driver_override;

    if ( brakes.enable == true )
    {
        report.enabled = 1;
    }
    else
    {
        report.enabled = 0;
    }

    report.pedal_input = ( uint16_t )brakes.can_pressure;
    report.pedal_command = ( uint16_t )brakes.pedal_command;
    report.pedal_output = ( uint16_t )brakes.pressure;

    CAN.sendMsgBuf( PS_CTRL_MSG_ID_BRAKE_REPORT, // CAN ID
                    0,                           // standard ID (not extended)
                    8,                           // dlc
                    (byte*)&report );            // brake report
}


// *****************************************************
// Function:    publish_timed_tx_frames
// 
// Purpose:     Determine if enough time has passed to publish the braking report
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void publish_timed_tx_frames( void )
{
    static uint32_t tx_timestamp = 0;

    uint32_t delta = timer_delta_ms( tx_timestamp );

    if ( delta >= PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL )
    {
        tx_timestamp = millis( );

        publish_ps_ctrl_brake_report( );
    }
}



// *****************************************************
// Function:    process_ps_ctrl_brake_command
// 
// Purpose:     Process a brake command message
// 
// Returns:     void
// 
// Parameters:  control_data -  pointer to a brake command control message
// 
// *****************************************************
void process_ps_ctrl_brake_command(
    const ps_ctrl_brake_command_msg * const control_data )
{
    if ( control_data->enabled == 1 )
    {
        brakes.enable_request = true;
    }

    if ( control_data->enabled == 0 )
    {
        brakes.enable_request = false;
    }

    brakes.pedal_command = control_data->pedal_command;

    DEBUG_PRINT( "pedal_command: " );
    DEBUG_PRINT( brakes.pedal_command );
}


// *****************************************************
// Function:    process_psvc_chassis_state1
// 
// Purpose:     Process the chassis state message
// 
// Returns:     void
// 
// Parameters:  chassis_data - pointer to a chassis state message that contains
//                             the brake pressure
// 
// *****************************************************
void process_psvc_chassis_state1(
    const psvc_chassis_state1_data_s * const chassis_data )
{
    brakes.can_pressure = chassis_data->brake_pressure;
}



// *****************************************************
// Function:    handle_ready_rx_frames
// 
// Purpose:     Parse received CAN data and redirect to correct
//              processing function
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void handle_ready_rx_frames( )
{
    if ( CAN.checkReceive( ) == CAN_MSGAVAIL )
    {
        can_frame_s rx_frame;

        memset( &rx_frame, 0, sizeof(rx_frame) );

        rx_frame.timestamp = millis( );

        CAN.readMsgBufID( (INT32U*) &rx_frame.id,
                          (INT8U*) &rx_frame.dlc,
                          (INT8U*) rx_frame.data );

        if ( rx_frame.id == PS_CTRL_MSG_ID_BRAKE_COMMAND )
        {
            brakes.rx_timestamp = millis( );

            process_ps_ctrl_brake_command(
                ( const ps_ctrl_brake_command_msg * const )rx_frame.data );
        }

        if ( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            process_psvc_chassis_state1( 
                ( const psvc_chassis_state1_data_s * const )rx_frame.data );
        }
    }
}


// *****************************************************
// Function:    brake_update
// 
// Purpose:     Update function for the brake module
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_update( )
{
    static float pressure_target = 0.0;
    static float pressure = 0.0;

    brake_update_pressure( );

    // ********************************************************************
    //
    // WARNING
    //
    // The ranges selected to do brake control are carefully tested to
    // ensure that the pressure actuated is not outside of the range of
    // what the brake module can handle. By changing any of this code you
    // risk attempting to actuate a pressure outside of the brake modules
    // valid range. Actuating a pressure outside of the modules valid
    // range will, at best, cause it to go into an unrecoverable fault
    // state. This is characterized by the accumulator "continuously
    // pumping" without accumulating any actual pressure, or being
    // "over pressured." Clearing this fault state requires expert
    // knowledge of the braking module.
    //
    // It is NOT recommended to modify any of the existing control ranges,
    // or gains, without expert knowledge.
    //
    // ************************************************************************

    static struct interpolate_range_s pressure_ranges =
        { 0.0, UINT16_MAX, 12.0, 878.3 };

    pressure = brakes.pressure;

    pressure_target = interpolate( brakes.pedal_command, &pressure_ranges );

    int16_t ret = pid_update( &pid_params, pressure_target, pressure, 0.01 );

    if ( ret == PID_SUCCESS )
    {
        float pid_output = pid_params.control;

        if ( pid_output < 0.0 )
        {
            static struct interpolate_range_s slr_ranges =
                { 0.0, 0.5, SLR_DUTY_CYCLE_MIN, SLR_DUTY_CYCLE_MAX };

            uint16_t slr_duty_cycle = 0;

            // pressure is too high
            brake_command_actuator_solenoids( 0 );

            pid_output = -pid_output;
            slr_duty_cycle = (uint16_t)interpolate( pid_output, &slr_ranges );

            if ( slr_duty_cycle > ( uint16_t )SLR_DUTY_CYCLE_MAX )
            {
                slr_duty_cycle = ( uint16_t )SLR_DUTY_CYCLE_MAX;
            }

            brake_command_release_solenoids( slr_duty_cycle );
        }
        else if ( pid_output > 0.0 )
        {
            static struct interpolate_range_s sla_ranges =
                { 0.0, 0.5, SLA_DUTY_CYCLE_MIN, SLA_DUTY_CYCLE_MAX };

            uint16_t sla_duty_cycle = 0;

            brake_lights_on();

            // pressure is too low
            brake_command_release_solenoids( 0 );

            sla_duty_cycle = (uint16_t)interpolate( pid_output, &sla_ranges );

            if ( sla_duty_cycle > ( uint16_t )SLA_DUTY_CYCLE_MAX )
            {
                sla_duty_cycle = ( uint16_t )SLA_DUTY_CYCLE_MAX;
            }

            brake_command_actuator_solenoids( sla_duty_cycle );
        }
        else    // pid_outout == 0.0
        {
            if ( brakes.pedal_command == 0 )
            {
                brake_lights_off();
            }
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
// Parameters:  void
// 
// *****************************************************
void check_rx_timeouts( )
{
    // local vars
    uint32_t delta = timer_delta_ms( brakes.rx_timestamp );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        brakes.enable_request = false;
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
//              Setup routine runs once you press reset
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void setup( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range 
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    accumulator_init( );
    master_cylinder_init( );
    brake_init( );

    // depower all the things
    accumulator_turn_pump_off( );
    master_cylinder_open( );

    brake_command_release_solenoids( 0 );
    brake_command_actuator_solenoids( 0 );

    init_serial( );
    init_can( );

    publish_ps_ctrl_brake_report( );

    // update last Rx timestamps so we don't set timeout warnings on start up
    brakes.rx_timestamp = millis( );

    // Initialize PID params
    pid_zeroize( &pid_params, BRAKE_PID_WINDUP_GUARD );

    pid_params.proportional_gain = 1.0;
    pid_params.integral_gain = 0.5;
    pid_params.derivative_gain = 0.5;

    DEBUG_PRINT( "init: pass" );
}


/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */

// *****************************************************
// Function:    loop
// 
// Purpose:     Handle and publish CAN frames
//              Check for receive timeouts
//              Check for driver brake override
//              Update the brakes
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void loop( )
{
    handle_ready_rx_frames( );

    publish_timed_tx_frames( );

    accumulator_maintain_pressure( );

    check_rx_timeouts( );

    brake_check_driver_override( );

    if ( brakes.enable != brakes.enable_request )
    {
        if ( brakes.enable_request == true )
        {
            brake_enable();
        }
        else
        {
            brake_disable();
        }
    }

    if ( brakes.enable == true )
    {
        static uint32_t control_loop_time = 0;

        uint32_t loop_delta_t = timer_delta_ms( control_loop_time );

        if ( loop_delta_t > BRAKE_CONTROL_LOOP_INTERVAL )
        {
            control_loop_time = millis();
            brake_update( );
        }
    }
}

