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

// Throttle control ECU firmware
// Firmware for control of 2014 Kia Soul throttle system
// Component
//    Arduino Mega
//    Seeed Studio CAN-BUS Shield, v1.2 (MCP2515)
//    Sainsmart 4 relay module
//    6 channel mosfet Board
// E Livingston, 2016

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
    #define DEBUG_PRINT( x )  Serial.println( x )
#else
    #define DEBUG_PRINT( x )
#endif


// *****************************************************
// Constants
// *****************************************************

// the following are guesses, these need to be debugged/researched
const float ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const float PRESSURE_STEP = 0.2;         // The amount that the 'a' and 'd' commands change the
                                         // voltage each time they are pressed.
const float MIN_ACCUMULATOR_PRESSURE = 2.3;              // minumum accumulator pressure to maintain
const float MAX_ACCUMULATOR_PRESSURE = 2.4;              // max accumulator pressure to maintain
const float PEDAL_THRESHOLD = 0.6;       // Pressure for pedal interference

// *****************************************************
// static global data/macros
// *****************************************************

#define CAN_SHIELD_CHIP_SELECT_PIN ( 53 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT ( 150 )

#define BRAKE_PID_WINDUP_GUARD ( 500 )

#define BRAKE_CONTROL_WAIT_STATE ( 0 )
#define BRAKE_CONTROL_BRAKE_STATE ( 1 )

// The min/max duty cycle scalars used for 3.921 KHz PWM frequency.
// These represent the minimum duty cycles that begin to actuate the
// proportional solenoids and the maximum duty cycle where the solenoids
// have reached their stops.
#define ACCUMULATOR_MAX_DUTY_CYCLE ( 105.0 )
#define ACCUMULATOR_MIN_DUTY_CYCLE ( 50.0 )
#define RELEASE_MAX_DUTY_CYCLE ( 100.0 )
#define RELEASE_MIN_DUTY_CYCLE ( 50.0 )


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

// brake structure
struct brake_data_s
{
    float pressure_left;    // last known right-side pressure
    float pressure_right;   // last known left-side pressure

    float pressure;
    float pressure_request;

    int16_t accumulator_duty_cycle;
    int16_t release_duty_cycle;

    uint8_t requested_state;
    uint8_t current_state;

    uint32_t rx_timestamp;

    int16_t driver_override;

    int16_t can_pressure;
    uint16_t pedal_command;
};


// *****************************************************
// static global declarations
// *****************************************************

// construct the CAN shield object
MCP_CAN CAN( CAN_SHIELD_CHIP_SELECT_PIN );  // Set CS pin for the CAN shield

static PID pid_params;


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
    0.0,                        // pressure_left
    0.0,                        // pressure_right
    0.0,                        // pressure
    0.0,                        // pressure_request
    0,                          // accumulator_duty_cycle
    0,                          // release_duty_cycle
    BRAKE_CONTROL_WAIT_STATE,   // requested_state
    BRAKE_CONTROL_WAIT_STATE,   // current_state
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
// Parameters:  None
// 
// *****************************************************
static void init_serial( void )
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
static void init_can( void )
{
    while ( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINT( "init_can: retrying" );
    }

    DEBUG_PRINT( "init_can: pass" );
}

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
        delta = ( UINT32_MAX - last_time ) + current_time;
    }   
    else
    {   
        delta = current_time - last_time;
    }
    return ( delta );
}


// *****************************************************
// Function:    calculate_accumulator_duty_cycle
// 
// Purpose:     Calculate pressure value
// 
// Returns:     void
// 
// Parameters:  float PID_output - the output of the PID loop
//              input is range limited between -2.0 to 2.0
// 
// *****************************************************
void calculate_accumulator_duty_cycle( float pid_output )
{
    float scaled = abs( pid_output ) * 512.0;

    if ( scaled > 1024.0 )
    {
        scaled = 1024.0;
    }

    scaled *= ( ACCUMULATOR_MAX_DUTY_CYCLE - ACCUMULATOR_MIN_DUTY_CYCLE ) / 1024.0;

    scaled += ACCUMULATOR_MIN_DUTY_CYCLE;

    brakes.accumulator_duty_cycle = ( int16_t )scaled;
}

// *****************************************************
// Function:    calculate_release_duty_cycle
// 
// Purpose:     Calculate pressure value
// 
// Returns:     void
// 
// Parameters:  float PID_output - the output of the PID loop
//              input is range limited between -2.0 to 2.0
// 
// *****************************************************
void calculate_release_duty_cycle( float pid_output )
{
    float scaled = abs( pid_output ) * 512.0;

    if ( scaled > 1024.0 )
    {
        scaled = 1024.0;
    }

    scaled *= ( RELEASE_MAX_DUTY_CYCLE - RELEASE_MIN_DUTY_CYCLE ) / 1024.0;

    scaled += RELEASE_MIN_DUTY_CYCLE;

    brakes.release_duty_cycle = ( int16_t )scaled;
}

// *****************************************************
// Function:    pressure_to_voltage
// 
// Purpose:     Convert pressure to voltage
// 
// Returns:     float - voltage
// 
// Parameters:  int16_t - pressure in MPa
// 
// *****************************************************
float pressure_to_voltage( int16_t pressure )
{
    float voltage = ( ( float )pressure + 217.1319446 ) / 505.5662053;
    return voltage;
}

// *****************************************************
// Function:    voltage_to_pressure
// 
// Purpose:     Convert voltage to pressure
// 
// Returns:     int16_t - pressure in MPa
// 
// Parameters:  float - volts
// 
// *****************************************************
int16_t voltage_to_pressure( float voltage )
{
    int16_t pressure = ( int16_t )( ( voltage * 505.5662053 ) - 217.1319446 );
    return pressure;
}

// *****************************************************
// Function:    raw_adc_to_voltage
// 
// Purpose:     Convert the raw ADC reading (0 - 1023)
//              to a pressure (0 - 5V)
// 
// Returns:     float - pressure
// 
// Parameters:  int16_t - raw ADC reading 
// 
// *****************************************************
float raw_adc_to_voltage( int16_t input )
{
    float voltage = ( ( float )input * ( 5.0 / 1023.0 ) );
    return voltage;
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
// Parameters:  None
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
// Parameters:  None
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
// Parameters:  None
// 
// *****************************************************
void accumulator_maintain_pressure( )
{
    const float accumulator_alpha = 0.05;

    int16_t raw_accumulator_data = analogRead( PIN_PACC );

    float pressure = raw_adc_to_voltage( raw_accumulator_data );

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
// Parameters:  None
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
// Parameters:  None
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
// Parameters:  uint8_t sensor1_pin
//              uint8_t sensor2_pin
//              uint8_t control_pin
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
// Function:    brake_request_wait_state
// 
// Purpose:     Set the requested state variable to WAIT
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_request_wait_state( )
{
    brakes.requested_state = BRAKE_CONTROL_WAIT_STATE;
}

// *****************************************************
// Function:    brake_request_brake_state
// 
// Purpose:     Set the requested state variable to BRAKE
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_request_brake_state( )
{
    brakes.requested_state = BRAKE_CONTROL_BRAKE_STATE;
}


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
// Parameters:  None
//
// *****************************************************
void brake_check_driver_override( )
{
    static const float filter_alpha = 0.05;
    static const float max_pedal_voltage = PEDAL_THRESHOLD;

    static float filtered_input_1 = 0.0;
    static float filtered_input_2 = 0.0;

    float sensor_1 = ( float )( analogRead( PIN_PMC1 ) );
    float sensor_2 = ( float )( analogRead( PIN_PMC2 ) );

    sensor_1 = raw_adc_to_voltage( sensor_1 );
    sensor_2 = raw_adc_to_voltage( sensor_2 );

    filtered_input_1 = ( filter_alpha * sensor_1 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_1 );

    filtered_input_2 = ( filter_alpha * sensor_2 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_2 );

    if ( ( filtered_input_1 > max_pedal_voltage ) ||
         ( filtered_input_2 > max_pedal_voltage ) )
    {
        brakes.pressure_request = ZERO_PRESSURE;
        brakes.driver_override = 1;
        brake_request_wait_state( );
    }
    else
    {
        brakes.driver_override = 0;
    }
}


// *****************************************************
// Function:    brake_turn_on_accumulator_solenoids
// 
// Purpose:     Turn on brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_turn_on_accumulator_solenoids( )
{
    analogWrite( PIN_SLAFL, brakes.accumulator_duty_cycle );
    analogWrite( PIN_SLAFR, brakes.accumulator_duty_cycle );
}


// *****************************************************
// Function:    brake_turn_off_accumulator_solenoids
// 
// Purpose:     Turn off brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_turn_off_accumulator_solenoids( )
{
    analogWrite( PIN_SLAFL, 0 );
    analogWrite( PIN_SLAFR, 0 );
}

// *****************************************************
// Function:    brake_turn_on_release_solenoids
// 
// Purpose:     Turn on brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  void
// 
// *****************************************************
void brake_turn_on_release_solenoids( )
{
    analogWrite( PIN_SLRFL, brakes.release_duty_cycle );
    analogWrite( PIN_SLRFR, brakes.release_duty_cycle );    
}

// *****************************************************
// Function:    brake_turn_off_release_solenoids
// 
// Purpose:     Turn off brake release solenoids
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_turn_off_release_solenoids( )
{
    digitalWrite( PIN_SLRFL, 0 );
    digitalWrite( PIN_SLRFR, 0 );
}

// *****************************************************
// Function:    brake_update_pressure
// 
// Purpose:     Update brake pressure
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_update_pressure( )
{
    uint16_t raw_left_pressure = analogRead( PIN_PFL );
    uint16_t raw_right_pressure = analogRead( PIN_PFR );

    brakes.pressure_left = raw_adc_to_voltage( raw_left_pressure );
    brakes.pressure_right = raw_adc_to_voltage( raw_right_pressure );

    brakes.pressure = ( brakes.pressure_left + brakes.pressure_right ) / 2;
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
// Parameters:  None
// 
// *****************************************************
static void publish_ps_ctrl_brake_report( )
{
    ps_ctrl_brake_report_msg report;

    report.override = ( uint8_t )brakes.driver_override;

    if ( brakes.current_state == BRAKE_CONTROL_BRAKE_STATE )
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
// Parameters:  None
// 
// *****************************************************
static void publish_timed_tx_frames( void )
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
static void process_ps_ctrl_brake_command(
    const ps_ctrl_brake_command_msg * const control_data )
{
    if ( control_data->enabled == 1 )
    {
        brake_request_brake_state();
    }

    if ( control_data->enabled == 0 )
    {
        brake_request_wait_state();
    }

    brakes.pedal_command = control_data->pedal_command;

    float pedal_command = ( float )brakes.pedal_command;

    // Scale 0->65535 input to 0.48->2.3 output
    pedal_command *= ( 2.3 - 0.48 ) / 65535.0;

    brakes.pressure_request = pedal_command + 0.48;

    DEBUG_PRINT( "pressure_request: " );
    DEBUG_PRINT( brakes.pressure_request );
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
static void process_psvc_chassis_state1(
    const psvc_chassis_state1_data_s * const chassis_data )
{
    brakes.can_pressure = chassis_data->brake_pressure;

    brake_update_pressure( );
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
// Function:    wait_state_enter
// 
// Purpose:     Enter wait state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_state_enter( )
{
    master_cylinder_open( );
    brake_turn_off_accumulator_solenoids( );
    brake_turn_off_release_solenoids( );

    DEBUG_PRINT( "Entered wait state" );
}


// *****************************************************
// Function:    wait_state_update
// 
// Purpose:     Update wait state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_state_update( )
{
    accumulator_maintain_pressure( );
}


// *****************************************************
// Function:    wait_state_exit
// 
// Purpose:     Exit wait state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_state_exit( )
{
}


// *****************************************************
// Function:    brake_enter
// 
// Purpose:     Enter brake state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_state_enter( )
{
    master_cylinder_close( );

    brake_turn_off_release_solenoids( );

    DEBUG_PRINT( "entered brake state" );
}


// *****************************************************
// Function:    brake_state_update
// 
// Purpose:     Update function for the brake state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_state_update( )
{
    static int16_t delta_t = 10;
    static int16_t curr_micros = 0;
    static int16_t last_micros = 0;

    static float pressure_last = 0;
    static float pressure_rate_target = 0;
    static float pressure_rate = 0;

    accumulator_maintain_pressure( );

    brake_update_pressure( );

    // ************************************************************************
    //
    // WARNING
    //
    // The ranges selected to do brake control are carefully tested to ensure
    // that the pressure actuated is not outside of the range of what the brake
    // module can handle. By changing any of this code you risk attempting to
    // actuate a pressure outside of the brake modules valid range. Actuating a
    // pressure outside of the modules valid range will, at best, cause it to
    // go into an unrecoverable fault state. This is characterized by the
    // accumulator "continuously pumping" without accumulating any actual
    // pressure, or being "over pressured." Clearing this fault state requires
    // expert knowledge of the braking module.
    //
    // It is NOT recommended to modify any of the existing control ranges, or
    // gains, without expert knowledge.
    //
    // ************************************************************************

    if ( brakes.pressure_request > ZERO_PRESSURE )
    {
        brake_lights_on();

        master_cylinder_close();

        last_micros = curr_micros;
        curr_micros = micros( );
        delta_t = curr_micros - last_micros;

        // calculate pressure rate in pressure/microsecond
        pressure_rate = ( brakes.pressure - pressure_last ) / delta_t;
        pressure_rate_target = brakes.pressure_request - brakes.pressure;

        int16_t ret = pid_update( &pid_params, pressure_rate_target, pressure_rate, 0.050 );

        if ( ret == PID_SUCCESS )
        {
            float pressure_pid_output = pid_params.control;

            pressure_pid_output = m_constrain( (float) (pressure_pid_output),
                                               (float) -2.0f,
                                               (float) 2.0f );

            if ( pressure_pid_output < -0.1 )
            {
                // pressure is too high
                calculate_release_duty_cycle( pressure_pid_output );

                brake_turn_off_accumulator_solenoids( );
                brake_turn_on_release_solenoids( );
            }

            if ( pressure_pid_output > 0.1 )
            {
                // pressure is too low
                calculate_accumulator_duty_cycle( pressure_pid_output );

                brake_turn_off_release_solenoids( );
                brake_turn_on_accumulator_solenoids( );
            }
        }
    }
    else if ( brakes.pressure_request <= ZERO_PRESSURE )
    {
        master_cylinder_open();
        brake_turn_off_accumulator_solenoids( );
        brake_turn_off_release_solenoids( );

        brake_lights_off();
    }
}


// *****************************************************
// Function:    brake_exit
// 
// Purpose:     Exit brake state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_state_exit( )
{
    master_cylinder_open( );

    brake_turn_off_accumulator_solenoids( );

    brake_lights_off( );
}


// *****************************************************
// Function:    brake_update
// 
// Purpose:     Braking state machine
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_state_machine_update( )
{
    if ( brakes.current_state == BRAKE_CONTROL_WAIT_STATE )
    {
        if ( brakes.requested_state == BRAKE_CONTROL_BRAKE_STATE )
        {
            brakes.current_state = BRAKE_CONTROL_BRAKE_STATE;

            wait_state_exit();
            brake_state_enter();
            brake_state_update();
        }
        else
        {
            wait_state_update();
        }
    }
    else if ( brakes.current_state == BRAKE_CONTROL_BRAKE_STATE )
    {
        if ( brakes.requested_state == BRAKE_CONTROL_WAIT_STATE )
        {
            brakes.current_state = BRAKE_CONTROL_WAIT_STATE;

            brake_state_exit();
            wait_state_enter();
            wait_state_update();
        }
        else
        {
            brake_state_update();
        }
    }
    else
    {
        brakes.current_state = BRAKE_CONTROL_WAIT_STATE;
        brakes.requested_state = BRAKE_CONTROL_WAIT_STATE;
        wait_state_enter();
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
    // local vars
    uint32_t delta = timer_delta_ms( brakes.rx_timestamp );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        brake_request_wait_state();
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
// Parameters:  None
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

    // close release solenoids
    brake_turn_off_release_solenoids( );

    // clear any accumulator pressure
    brake_turn_on_accumulator_solenoids( );

    delay(3000);

    brake_turn_off_accumulator_solenoids( );

    // Initialize serial devices (RS232 and CAN)
    init_serial( );

    init_can( );

    publish_ps_ctrl_brake_report( );

    // update last Rx timestamps so we don't set timeout warnings on start up
    brakes.rx_timestamp = millis( );

    // Initialize PID params
    pid_zeroize( &pid_params, BRAKE_PID_WINDUP_GUARD );

    pid_params.derivative_gain = 0.50;
    pid_params.proportional_gain = 10.0;
    pid_params.integral_gain = 1.5;

    // debug log
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
//              Update the brake state machine
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

    brake_check_driver_override( );

    brake_state_machine_update( );
}
