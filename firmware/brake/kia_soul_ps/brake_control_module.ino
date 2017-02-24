/************************************************************************/
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

// Brake control ECU firmware
// 2004-2009 Prius brake actuator




#include <SPI.h>
#include <FiniteStateMachine.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"
#include "control_protocol_can.h"
#include "PID.h"

// *****************************************************
// Constants
// *****************************************************

// the following are guesses, these need to be debugged/researched
const float ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const float PRESSURE_STEP = 0.2;         // The amount that the 'a' and 'd' commands change the
                                         // voltage each time they are pressed.
const float MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const float MAX_PACC = 2.4;              // max accumulator pressure to maintain
const float PEDAL_THRESHOLD = 0.6;       // Pressure for pedal interference

// *****************************************************
// static global data/macros
// *****************************************************

#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
    #warning "PSYNC_DEBUG_FLAG defined"
    #define DEBUG_PRINT( x )  Serial.println( x )
#else
    #define DEBUG_PRINT( x )
#endif

// chip select pin for CAN Shield
#define CAN_CS ( 53 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT ( 250 )

// Braking PID windup guard
#define BRAKE_PID_WINDUP_GUARD ( 500 )

//
static uint32_t last_update_ms;


// *****************************************************
// static structures
// *****************************************************


// construct the CAN shield object
MCP_CAN CAN( CAN_CS );                                    // Set CS pin for the CAN shield

//
static can_frame_s rx_frame_ps_ctrl_brake_command;

//
static can_frame_s tx_frame_ps_ctrl_brake_report;

//
static PID pid_params;


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
// local declarations
// *****************************************************
void wait_enter( );
void wait_update( );
void wait_exit( );

void brake_enter( );
void brake_update( );
void brake_exit( );


// *****************************************************
// local definitions
// *****************************************************
float pressure_req;
float pressure;

uint8_t incoming_serial_byte;

uint32_t previous_millis = 0;

bool control_enabled = false;
int16_t driver_override = 0;

int16_t solenoid_left_actuation_duty_max;
int16_t solenoid_left_actuation_duty_min;
int16_t solenoid_left_release_duty_max;
int16_t solenoid_left_release_duty_min;

// *****************************************************
// Function:    calculate_solenoid_left_actuation_duty_cycle
// 
// Purpose:     Calculate pressure value
// 
// Returns:     int16_t - the scaled pressure value
// 
// Parameters:  float PID_output - the output of the PID loop
// 
// *****************************************************
int16_t calculate_solenoid_left_actuation_duty_cycle( float PID_output )
{
  int16_t scaled = ( ( int16_t )abs( PID_output ) ) * 512;
  int16_t scale =  map( scaled, 0, 1024, solenoid_left_actuation_duty_min, solenoid_left_actuation_duty_max );
  return scale;
}

// *****************************************************
// Function:    calculate_solenoid_left_release_duty_cycle
// 
// Purpose:     Calculate pressure value
// 
// Returns:     int16_t - the scaled pressure value
// 
// Parameters:  float PID_output - the output of the PID loop
// 
// *****************************************************
int16_t calculate_solenoid_left_release_duty_cycle( float PID_output )
{
  int16_t scaled = ( ( int16_t )abs( PID_output ) ) * 512;
  int16_t scale =  map( scaled, 0, 1024, solenoid_left_release_duty_min, solenoid_left_release_duty_max );
  return scale;
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
    int16_t pressure = (int16_t)( ( voltage * 505.5662053 ) - 217.1319446 );
    return pressure;
}

// *****************************************************
// Function:    convert_ADC_reading_to_pressure
// 
// Purpose:     Convert the ADC reading
//              (which goes from 0-1023)
//              to a pressure (0 - 5V)
// 
// Returns:     float - pressure
// 
// Parameters:  int16_t - ADC reading 
// 
// *****************************************************
float convert_ADC_reading_to_pressure( int16_t input )
{
    float voltage = ( ( float )input * ( 5.0 / 1023.0 ) );
    return voltage;
}

// *****************************************************
// MOSFET pin (digital) definitions ( MOSFETs control the solenoids )
//
// Pins are not perfectly sequential because the clock frequency of certain pins is different.
//
// *****************************************************
// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
const uint8_t PIN_SLAFL = 5;      // front left actuation
const uint8_t PIN_SLAFR = 7;      // front right actuation

// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const uint8_t PIN_SLRFL = 6;      // front left return
const uint8_t PIN_SLRFR = 8;      // front right return
const uint8_t PIN_SMC   = 2;      // master cylinder solenoids (two of them)

const uint8_t PIN_BRAKE_SWITCH = 48;
const uint8_t PIN_PUMP         = 49;     // accumulator pump motor

// sensor pin (analog) definitions
const uint8_t PIN_PACC = 9;       // pressure accumulator sensor
const uint8_t PIN_PMC1 = 10;      // pressure master cylinder sensor 1
const uint8_t PIN_PMC2 = 11;      // pressure master cylinder sensor 2
const uint8_t PIN_PFR  = 13;      // pressure front right sensor
const uint8_t PIN_PFL  = 14;      // pressure front left sensor

// *****************************************************
// Initialize states
// *****************************************************
State waiting_state = State( wait_enter, wait_update, wait_exit );        // Wait for brake instructions
State braking_state = State( brake_enter, brake_update, brake_exit );    // Control braking

// initialize state machine, start in state: Wait
FSM brake_state_machine = FSM( waiting_state );

// *****************************************************
// local data structures and associated functions
// *****************************************************
struct accumulator_data_s
{
    float pressure = 0.0;
    uint8_t sensor_pin = 99;
    uint8_t control_pin = 99;
};

struct accumulator_data_s accumulator;

// *****************************************************
// Function:    turn_accumulator_pump_off
// 
// Purpose:     Turn off accumulator pump
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void turn_accumulator_pump_off( )
{
    digitalWrite( accumulator.control_pin, LOW );
}

// *****************************************************
// Function:    update_accumulator_pressure
// 
// Purpose:     Update accumulator pressure
//              TODO: Currently does nothing
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void update_accumulator_pressure( )
{

}

// *****************************************************
// Function:    turn_accumulator_pump_on
// 
// Purpose:     Turn on accumulator pump
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void turn_accumulator_pump_on( )
{
    digitalWrite( accumulator.control_pin, HIGH );
}

// *****************************************************
// Function:    maintain_accumulator_pressure
// 
// Purpose:     Turn accumulator pump on or off
//              to maintain pressure
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void maintain_accumulator_pressure( )
{
    int16_t raw_accumulator_data = analogRead( accumulator.sensor_pin );
    accumulator.pressure = convert_ADC_reading_to_pressure( raw_accumulator_data );

    if ( accumulator.pressure < MIN_PACC )
    {
        turn_accumulator_pump_on( );
    }
    else
    {
        if ( accumulator.pressure > MAX_PACC )
        {
            turn_accumulator_pump_off( );
        }
    }
}

// *****************************************************
// Function:    init_accumulator
// 
// Purpose:     Initializes the accumulator
// 
// Returns:     void
// 
// Parameters:  uint8_t sensor_pin
//              uint8_t control_pin
// 
// *****************************************************
void init_accumulator( uint8_t sensor_pin, uint8_t control_pin )
{
    accumulator.sensor_pin = sensor_pin;
    accumulator.control_pin = control_pin;

    pinMode( accumulator.control_pin, OUTPUT );

    turn_accumulator_pump_off( );
}

//master solenoid structure
struct master_cylinder_solenoid_data_s
{
    float pressure1 = 0.0; // Initialize pressures to 0.0 to avoid false values
    float pressure2 = 0.0;
    uint8_t sensor1_pin = 99;
    uint8_t sensor2_pin = 99;
    uint8_t control_pin = 99;
};

struct master_cylinder_solenoid_data_s master_cylinder_solenoid;

// *****************************************************
// Function:    open_master_cylinder_solenoid
// 
// Purpose:     Open master cylinder solenoid
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void open_master_cylinder_solenoid( )
{
    analogWrite( master_cylinder_solenoid.control_pin, 0 );
}

// *****************************************************
// Function:    close_master_cylinder_solenoid
// 
// Purpose:     Close master cylinder solenoid
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void close_master_cylinder_solenoid( )
{
    analogWrite( master_cylinder_solenoid.control_pin, 255 );
}

// *****************************************************
// Function:    check_driver_brake_override
// 
// Purpose:     Check if driver is pressing the brake
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void check_driver_brake_override( )
{
    int16_t raw_ADC_sensor1_data = analogRead( master_cylinder_solenoid.sensor1_pin );
    master_cylinder_solenoid.pressure1 = convert_ADC_reading_to_pressure( raw_ADC_sensor1_data );
    int16_t raw_ADC_sensor2_data = analogRead( master_cylinder_solenoid.sensor2_pin );
    master_cylinder_solenoid.pressure2 = convert_ADC_reading_to_pressure( raw_ADC_sensor2_data );

    //if current pedal pressure is greater than the limit
    //(because of driver override by pressing the brake pedal), disable.
    if ( ( master_cylinder_solenoid.pressure1 > PEDAL_THRESHOLD ) ||
        ( master_cylinder_solenoid.pressure2 > PEDAL_THRESHOLD ) )
    {
        DEBUG_PRINT( "Brake Pedal Detected" );
        pressure_req = ZERO_PRESSURE;
        driver_override = 1;
        brake_state_machine.transitionTo( waiting_state );
    }
    else
    {
        driver_override = 0;
    }
}

// *****************************************************
// Function:    init_master_cylinder_solenoid
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
void init_master_cylinder_solenoid( uint8_t sensor1_pin, uint8_t sensor2_pin, uint8_t control_pin )
{
    master_cylinder_solenoid.sensor1_pin = sensor1_pin;
    master_cylinder_solenoid.sensor2_pin = sensor2_pin;
    master_cylinder_solenoid.control_pin = control_pin;

    pinMode( master_cylinder_solenoid.control_pin, OUTPUT );

    open_master_cylinder_solenoid( );
}

//brake structure
struct brake_data_s
{
    float pressure_left = 0.0;                  // last known right-side pressure
    float pressure_right = 0.0;                 // last known left-side pressure
    uint8_t sensor_pin_left = 99;               // pin associated with left-side  pressure sensor
    uint8_t sensor_pin_right = 99;              // pin associated with right-side pressure sensors
    uint8_t solenoid_left_actuation = 99;       // pin associated with MOSFET, associated with actuation solenoid
    uint8_t solenoid_right_actuation = 99;      // pin associated with MOSFET, associated with actuation solenoid
    uint8_t solenoid_left_release = 99;         // pin associated with MOSFET, associated with release solenoid
    uint8_t solenoid_right_release = 99;        // pin associated with MOSFET, associated with release solenoid
    bool increasing_pressure = false;           // used to track if pressure should be increasing
    bool decreasing_pressure = false;           // used to track if pressure should be decreasing
    uint32_t previous_millis = 0;               // will store last time solenoid was updated
};

struct brake_data_s brakes;

// *****************************************************
// Function:    turn_off_all_brake_solenoids
// 
// Purpose:     Turn off all brake solenoids
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void turn_off_all_brake_solenoids( )
{
    analogWrite( brakes.solenoid_left_actuation, 0 );
    analogWrite( brakes.solenoid_right_actuation, 0 );
    analogWrite( brakes.solenoid_left_release, 0 );
    analogWrite( brakes.solenoid_right_release, 0 );
}

// *****************************************************
// Function:    turn_on_brake_actuator_solenoids
// 
// Purpose:     Turn on brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  int16_t scaled_pressure_value
// 
// *****************************************************
void turn_on_brake_actuator_solenoids( int16_t scaled_pressure_value )
{
    analogWrite( brakes.solenoid_left_actuation, scaled_pressure_value );
    analogWrite( brakes.solenoid_right_actuation, scaled_pressure_value );
}

// *****************************************************
// Function:    turn_off_brake_actuator_solenoids
// 
// Purpose:     Turn off brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void turn_off_brake_actuator_solenoids( )
{
    analogWrite( brakes.solenoid_left_actuation, 0 );
    analogWrite( brakes.solenoid_right_actuation, 0 );
}

// *****************************************************
// Function:    turn_on_brake_release_solenoids
// 
// Purpose:     Turn on brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  int16_t scalar to write to solenoids
// 
// *****************************************************
void turn_on_brake_release_solenoids( int16_t scaled_pressure_value )
{
    analogWrite( brakes.solenoid_left_release, scaled_pressure_value );
    analogWrite( brakes.solenoid_right_release, scaled_pressure_value );    
}

// *****************************************************
// Function:    turn_off_brake_release_solenoids
// 
// Purpose:     Turn off brake release solenoids
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void turn_off_brake_release_solenoids( )
{
    digitalWrite( brakes.solenoid_left_release, LOW );
    digitalWrite( brakes.solenoid_right_release, LOW );
}

// *****************************************************
// Function:    update_brake_pressure
// 
// Purpose:     Update brake pressure
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void update_brake_pressure( )
{
    brakes.pressure_left = convert_ADC_reading_to_pressure( analogRead( brakes.sensor_pin_left ) );
    brakes.pressure_right = convert_ADC_reading_to_pressure( analogRead( brakes.sensor_pin_right ) );  
}

// *****************************************************
// Function:    init_brakes
// 
// Purpose:     Initializes the brakes
// 
// Returns:     void
// 
// Parameters:  struct brake_init_data data to initialze brake structure
// 
// *****************************************************
void init_brakes( const struct brake_data_s * const brake_init_data )
{
    if ( brake_init_data != NULL )
    {
        brakes.sensor_pin_left = brake_init_data->sensor_pin_left;
        brakes.sensor_pin_right = brake_init_data->sensor_pin_right;
        brakes.solenoid_left_actuation = brake_init_data->solenoid_left_actuation;
        brakes.solenoid_right_actuation = brake_init_data->solenoid_right_actuation;
        brakes.solenoid_left_release = brake_init_data->solenoid_left_release;
        brakes.solenoid_right_release = brake_init_data->solenoid_right_release;

        // initialize solenoid pins to off
        digitalWrite( brakes.solenoid_left_actuation, LOW );
        digitalWrite( brakes.solenoid_right_actuation, LOW );
        digitalWrite( brakes.solenoid_left_release, LOW );
        digitalWrite( brakes.solenoid_right_release, LOW );

        // set pinmode to OUTPUT
        pinMode( brakes.solenoid_left_actuation, OUTPUT );
        pinMode( brakes.solenoid_right_actuation, OUTPUT );
        pinMode( brakes.solenoid_left_release, OUTPUT );
        pinMode( brakes.solenoid_right_release, OUTPUT );
    }
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
    // Wait until we have initialized
    while( CAN.begin( CAN_BAUD ) != CAN_OK )
    {
        // wait a little
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINT( "init_can: retrying" );
    }

    // Debug log
    DEBUG_PRINT( "init_can: pass" );
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
static void publish_ps_ctrl_brake_report( void )
{
    // cast data
    ps_ctrl_brake_report_msg * data =
            ( ps_ctrl_brake_report_msg* ) tx_frame_ps_ctrl_brake_report.data;

    // set frame ID
    tx_frame_ps_ctrl_brake_report.id = ( uint32_t ) ( PS_CTRL_MSG_ID_BRAKE_REPORT );

    // set DLC
    tx_frame_ps_ctrl_brake_report.dlc = 8; //TODO

    // Set override flag
    data->override = driver_override;

    // publish to control CAN bus
    CAN.sendMsgBuf(
            tx_frame_ps_ctrl_brake_report.id,
            0, // standard ID (not extended)
            tx_frame_ps_ctrl_brake_report.dlc,
            tx_frame_ps_ctrl_brake_report.data );

    // update last publish timestamp, ms
    tx_frame_ps_ctrl_brake_report.timestamp = millis( );
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
    uint32_t delta = 
        timer_delta_ms( tx_frame_ps_ctrl_brake_report.timestamp );

    if ( delta >= PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL )
    {
        publish_ps_ctrl_brake_report( );
    }
}


uint16_t map_uint16(uint16_t x, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);


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
    // enable control from the MKZ interface
    if ( ( control_data->enabled == 1 ) &&
        ( control_enabled == false) )
    {
        control_enabled = true;
        brake_state_machine.transitionTo( braking_state );
    }

    // disable control from the MKZ interface
    if ( (control_data->enabled == 0) &&
        ( control_enabled == true) )
    {
        control_enabled = false;
        brake_state_machine.transitionTo( waiting_state );
    }

    rx_frame_ps_ctrl_brake_command.timestamp = millis( );

    uint16_t pedal_command = control_data->pedal_command;
    pressure_req = map(pedal_command, 0, 65535, 48, 230); // map to voltage range
    pressure_req = pressure_req / 100;
    DEBUG_PRINT("pressure_req: ");
    DEBUG_PRINT(pressure_req);
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
    // take a reading from the brake pressure sensors
    update_brake_pressure( );

    // average the pressure of the rear and front lines
    float pressure = ( brakes.pressure_left + brakes.pressure_right ) / 2.0;
    DEBUG_PRINT( pressure );
    DEBUG_PRINT( "," );
    DEBUG_PRINT( chassis_data->brake_pressure );
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
void handle_ready_rx_frames( void )
{
    if ( CAN.checkReceive( ) == CAN_MSGAVAIL )
    {
        can_frame_s rx_frame;

        memset( &rx_frame, 0, sizeof(rx_frame) );

        // update timestamp
        rx_frame.timestamp = millis( );

        // read frame
        CAN.readMsgBufID(
                (INT32U*) &rx_frame.id,
                (INT8U*) &rx_frame.dlc,
                (INT8U*) rx_frame.data );

        // check for a supported frame ID
        if ( rx_frame.id == PS_CTRL_MSG_ID_BRAKE_COMMAND )
        {
            // process brake command
            process_ps_ctrl_brake_command(
                ( const ps_ctrl_brake_command_msg * const )rx_frame.data );
        }

        // check for a supported frame ID
        if ( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
        {
            // process brake command
            process_psvc_chassis_state1(
                ( const psvc_chassis_state1_data_s * const )rx_frame.data );
        }
    }
}


// *****************************************************
// Function:    wait_enter
// 
// Purpose:     Enter wait state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_enter( )
{
    control_enabled = false;

    // open master cylinder solenoids
    open_master_cylinder_solenoid( );
    turn_off_brake_actuator_solenoids( );
    turn_off_brake_release_solenoids( );

    DEBUG_PRINT( "Entered wait state" );
}


// *****************************************************
// Function:    wait_update
// 
// Purpose:     Update wait state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_update( )
{
    // keep accumulator pressurized
    maintain_accumulator_pressure( );

    // TODO: Is this check needed? Don't we force transition elsewhere?
    if ( ( pressure_req > ZERO_PRESSURE + .01 ) &&
        ( control_enabled == true ) )
    {
        brake_state_machine.transitionTo( braking_state );
    }
}


// *****************************************************
// Function:    wait_exit
// 
// Purpose:     Exit wait state
//              TODO: Currently does nothing
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void wait_exit( )
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
void brake_enter( )
{
    // close master cylinder solenoids because they'll spill back to the reservoir
    close_master_cylinder_solenoid( );

    // close solenoid_left_releases, they are normally open for failsafe conditions
    turn_off_brake_release_solenoids( );

    DEBUG_PRINT( "entered brake state" );
}


// *****************************************************
// Function:    brake_update
// 
// Purpose:     Update brake state
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void brake_update( )
{
    static int16_t delta_t = 10;
    static int16_t curr_micros = 0;
    static int16_t last_micros = 0;

    static float pressure_last = 0;
    static float pressure_rate_target = 0;
    static float pressure_rate = 0;
    
    // maintain accumulator pressure
    maintain_accumulator_pressure( );

    // take a reading from the brake pressure sensors
    update_brake_pressure( );
    pressure = ( brakes.pressure_left + brakes.pressure_right ) / 2;

    if ( pressure_req > ZERO_PRESSURE )
    {

/*******************************************************************************
*   WARNING
*
*   The ranges selected to do brake control are carefully tested to ensure that
*   the pressure actuated is not outside of the range of what the brake module
*   can handle. By changing any of this code you risk attempting to actuate
*   a pressure outside of the brake modules valid range. Actuating a pressure
*   outside of the modules valid range will, at best, cause it to go into an
*   unrecoverable fault state. This is characterized by the accumulator
*   "continuously pumping" without accumulating any actual pressure, or being
*   "over pressured." Clearing this fault state requires expert knowledge of the
*   breaking module.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/
	    
        digitalWrite( PIN_BRAKE_SWITCH, HIGH );
        close_master_cylinder_solenoid();
        
        // calculate a delta t
        last_micros = curr_micros;
        curr_micros = micros( );  // Fast loop, needs more precision than millis
        delta_t = curr_micros - last_micros;

        pressure_rate = ( pressure - pressure_last)/ delta_t;  // pressure/microsecond
        pressure_rate_target = pressure_req - pressure;

        pid_params.derivative_gain = 0.50;
        pid_params.proportional_gain = 10.0;
        pid_params.integral_gain = 1.5;

        int16_t ret = pid_update( &pid_params, pressure_rate_target, pressure_rate, 0.050 );

        if ( ret == PID_SUCCESS )
        {
            float pressure_pid_output = pid_params.control;

            // constrain to min/max
            pressure_pid_output = m_constrain(
                    (float) (pressure_pid_output),
                    (float) -2.0f,
                    (float) 2.0f );

            // some logic to set a samplerate for data which is sent to processing for plotting
            uint32_t current_millis = millis();
            if ( ( uint32_t )( current_millis - previous_millis ) >= 100 )
            {
                previous_millis = current_millis;
            }

            // if pressure is too high
            if ( pressure_pid_output < -0.1 )
            {
                turn_off_brake_actuator_solenoids( );
                turn_on_brake_release_solenoids( calculate_solenoid_left_release_duty_cycle( pressure_pid_output ) );
            }

            // if pressure is too low
            if ( pressure_pid_output > 0.1 )
            {
                turn_off_brake_release_solenoids( );
                turn_on_brake_actuator_solenoids( calculate_solenoid_left_actuation_duty_cycle( pressure_pid_output ) );
            }

            // if driver is not braking, transition to wait state
            if ( pressure_req <= ZERO_PRESSURE )
            {
                DEBUG_PRINT( "pressure request below threshold" );
                brake_state_machine.transitionTo( waiting_state );
            }
        }
    }
    else if ( pressure_req <= ZERO_PRESSURE ) 
    {
        open_master_cylinder_solenoid();
        turn_off_brake_actuator_solenoids();
        turn_off_brake_release_solenoids();

        // unswitch brake switch
        digitalWrite( PIN_BRAKE_SWITCH, LOW );
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
void brake_exit( )
{
    // close master cylinder solenoids
    open_master_cylinder_solenoid( );

    // depower wheel solenoids to vent brake pressure at wheels
    turn_off_brake_actuator_solenoids( );

    // unswitch brake switch
    digitalWrite( PIN_BRAKE_SWITCH, LOW );
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
    uint32_t delta = 
        timer_delta_ms( rx_frame_ps_ctrl_brake_command.timestamp );

    if ( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        // disable control from the PolySync interface
        if ( control_enabled )
        {
            control_enabled = false;
            brake_state_machine.transitionTo( waiting_state );
        }
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

    // set the min/max duty cycle scalers used for 3.921 KHz PWM frequency.
    // These represent the minimum duty cycles that begin to actuate the proportional solenoids 
    // and the maximum dudty cycle where the solenoids have reached their stops.
    solenoid_left_actuation_duty_max = 105;
    solenoid_left_actuation_duty_min = 50;
    solenoid_left_release_duty_max = 100;
    solenoid_left_release_duty_min = 50;

    //Initialize structs
    init_accumulator( PIN_PACC, PIN_PUMP );
    init_master_cylinder_solenoid( PIN_PMC1, PIN_PMC2, PIN_SMC );
    struct brake_data_s local_brake_data;

    local_brake_data.sensor_pin_left = PIN_PFL;
    local_brake_data.sensor_pin_right = PIN_PFR;
    local_brake_data.solenoid_left_actuation = PIN_SLAFL;
    local_brake_data.solenoid_right_actuation = PIN_SLAFR;
    local_brake_data.solenoid_left_release = PIN_SLRFL;
    local_brake_data.solenoid_right_release = PIN_SLRFR;

    init_brakes( &local_brake_data );

    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_brake_command, 0, sizeof( rx_frame_ps_ctrl_brake_command ) );

    digitalWrite( PIN_BRAKE_SWITCH, LOW );
    pinMode( PIN_BRAKE_SWITCH, OUTPUT );

    // depower all the things
    turn_accumulator_pump_off( );
    open_master_cylinder_solenoid( );

    // close rear solenoid_left_releases. These should open only for emergencies and to release brake pressure
    turn_off_brake_release_solenoids( );
    turn_off_brake_actuator_solenoids( );

    init_serial( );

    init_can( );

    publish_ps_ctrl_brake_report( );

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_brake_command.timestamp = millis( );

    // update the global system update timestamp, ms
    last_update_ms = millis( );

    // Initialize PID params
    pid_zeroize( &pid_params, BRAKE_PID_WINDUP_GUARD );

    // debug log
    DEBUG_PRINT( "init: pass" );
}


/* ====================================== */
/* ================ LOOP ================ */
/* ====================================== */

// *****************************************************
// Function:    loop
// 
// Purpose:     Update timestamp
//              Handle and publish CAN frames
//              Check for timeouts
//              Check for driver brake override
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void loop( )
{
    // update the global system update timestamp, ms
    last_update_ms = millis( );

    handle_ready_rx_frames( );

    publish_timed_tx_frames( );

    check_rx_timeouts( );

    // check pressures on master cylinder (pressure from pedal)
    check_driver_brake_override( );

    brake_state_machine.update( );
}
