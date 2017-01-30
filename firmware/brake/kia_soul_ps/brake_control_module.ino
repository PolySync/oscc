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

// Throttle control ECU firmware
// Firmware for control of 2014 Kia Soul throttle system
// Component
//    Arduino Mega
//    Seeed Studio CAN-BUS Shield, v1.2 (MCP2515)
//    Sainsmart 4 relay module
//    6 channel mosfet Board
// E Livingston, 2016

#include <SPI.h>
#include <FiniteStateMachine.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"
#include "control_protocol_can.h"
#include "PID.h"


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
#define CAN_CS ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT ( 150 )

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
double pressure_req;
double pressure;

uint8_t incoming_serial_byte;

uint32_t previous_millis = 0;

bool control_enabled = false;
int16_t local_override = 0;

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
// Parameters:  float pre - the pressure value
// 
// *****************************************************
int16_t calculate_solenoid_left_actuation_duty_cycle( float pre )
{
  int16_t scaled = abs( pre ) * 512;
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
// Parameters:  float pre - the pressure value
// 
// *****************************************************
int16_t calculate_solenoid_left_release_duty_cycle( float pre )
{
  int16_t scaled = abs( pre ) * 512;
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
float pressure_to_voltage( int16_t MPa )
{
    return ( MPa + 217.1319446 ) / 505.5662053;
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
    return ( voltage * 505.5662053 ) - 217.1319446;
}

// *****************************************************
// Function:    convert_to_voltage
// 
// Purpose:     Convert the ADC reading
//				(which goes from 0-1023)
//				to a voltage (0 - 5V)
// 
// Returns:     float - volts
// 
// Parameters:  int16_t - ADC reading 
// 
// *****************************************************
float convert_to_voltage( int16_t input )
{
    return input * ( 5.0 / 1023.0 );
}

// *****************************************************
// MOSFET pin (digital) definitions ( MOSFETs control the solenoids )
//
// Pins are not perfectly sequential because the clock frequency of certain pins is different.
//
// *****************************************************
// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
const uint8_t PIN_SLAFL = 3;      // front left actuation
const uint8_t PIN_SLAFR = 5;      // front right actuation
// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const uint8_t PIN_SLRFL = 6;      // front left return
const uint8_t PIN_SLRFR = 7;      // front right return
const uint8_t PIN_SMC = 8;      // master cylinder solenoids (two of them)

const uint8_t PIN_PUMP = 9;     // accumulator pump motor

// brake spoofer relay pin definitions
const uint8_t PIN_BRAKE_SWITCH_1 = 48;
const uint8_t PIN_BRAKE_SWITCH_2 = 49;

// sensor pin (analog) definitions
const uint8_t PIN_PACC = 9;       // pressure accumulator sensor
const uint8_t PIN_PMC1 = 10;      // pressure master cylinder sensor 1
const uint8_t PIN_PMC2 = 11;      // pressure master cylinder sensor 2
const uint8_t PIN_PRL = 12;       // pressure rear left sensor
const uint8_t PIN_PFR = 13;       // pressure front right sensor
const uint8_t PIN_PFL = 14;       // pressure front left sensor
const uint8_t PIN_PRR = 15;       // pressure rear right sensor

// the following are guesses, these need to be debugged/researched
const double ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const double PRESSURE_STEP = 0.2;         // The amount that the 'a' and 'd' commands change the
                                          // voltage each time they are pressed.
const double MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const double MAX_PACC = 2.4;              // max accumulator pressure to maintain
const double PEDAL_THRESHOLD = 0.5;          // Pressure for pedal interference

// *****************************************************
// Initialize states
// *****************************************************
State Wait = State( wait_enter, wait_update, wait_exit );        // Wait for brake instructions
State Brake = State( brake_enter, brake_update, brake_exit );    // Control braking

// initialize state machine, start in state: Wait
FSM brake_state_machine = FSM( Wait );

// *****************************************************
// local data structures and associated functions
// *****************************************************
struct accumulator_data_s
{
	float _pressure = 0.0;
	uint8_t _sensor_pin = 99;
	uint8_t _control_pin = 99;
} accumulator;

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
	digitalWrite( accumulator._control_pin, LOW );
}

// *****************************************************
// Function:    update_accumulator_pressure
// 
// Purpose:     Update accumulator pressure
//				TODO: Currently does nothing
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
	digitalWrite( accumulator._control_pin, HIGH );
}

// *****************************************************
// Function:    maintain_accumulator_pressure
// 
// Purpose:     Turn accumulator pump on or off
//				to maintain pressure
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void maintain_accumulator_pressure( )
{
	accumulator._pressure = convert_to_voltage( analogRead( accumulator._sensor_pin ) );

	if( accumulator._pressure < MIN_PACC )
	{
		turn_accumulator_pump_on( );
	}

	if( accumulator._pressure > MAX_PACC )
	{
		turn_accumulator_pump_off( );
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
//				uint8_t control_pin
// 
// *****************************************************
void init_accumulator( uint8_t sensor_pin, uint8_t control_pin )
{
	accumulator._sensor_pin = sensor_pin;
	accumulator._control_pin = control_pin;

	pinMode( accumulator._control_pin, OUTPUT );

	turn_accumulator_pump_off( );
}

//master solenoid structure
struct master_cylinder_solenoid_data_s
{
	float _pressure1 = 0.0; // Initialize pressures to 0.0 to avoid false values
    float _pressure2 = 0.0;
    uint8_t _sensor1_pin = 99;
    uint8_t _sensor2_pin = 99;
    uint8_t _control_pin = 99;
} master_cylinder_solenoid;

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
	analogWrite( master_cylinder_solenoid._control_pin, 0 );
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
	analogWrite( master_cylinder_solenoid._control_pin, 255 );
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
	master_cylinder_solenoid._pressure1 = convert_to_voltage( analogRead( master_cylinder_solenoid._sensor1_pin ) );
	master_cylinder_solenoid._pressure2 = convert_to_voltage( analogRead( master_cylinder_solenoid._sensor2_pin ) );

	//if current pedal pressure is greater than the limit
	//(because of driver override by pressing the brake pedal), disable.
	if( ( master_cylinder_solenoid._pressure1 > PEDAL_THRESHOLD ) ||
		( master_cylinder_solenoid._pressure2 > PEDAL_THRESHOLD ) )
	{
		DEBUG_PRINT( "Brake Pedal Detected" );
		pressure_req = ZERO_PRESSURE;
		local_override = 1;
		brake_state_machine.transitionTo( Wait );
	}
	else
	{
		local_override = 0;
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
//				uint8_t sensor2_pin
//				uint8_t control_pin
// 
// *****************************************************
void init_master_cylinder_solenoid( uint8_t sensor1_pin, uint8_t sensor2_pin, uint8_t control_pin )
{
	master_cylinder_solenoid._sensor1_pin = sensor1_pin;
	master_cylinder_solenoid._sensor2_pin = sensor2_pin;
	master_cylinder_solenoid._control_pin = control_pin;

	pinMode( master_cylinder_solenoid._control_pin, OUTPUT );

	open_master_cylinder_solenoid( );
}

//brake structure
struct brake_data_s
{
	float _pressure_left = 0.0;            // last known right-side pressure
    float _pressure_right = 0.0;           // last known left-side pressure
    uint8_t _sensor_pin_left = 99;            // pin associated with left-side  pressure sensor
    uint8_t _sensor_pin_right = 99;           // pin associated with right-side pressure sensors
    uint8_t _solenoid_pin_left_a = 99;        // pin associated with MOSFET, associated with actuation solenoid
    uint8_t _solenoid_pin_right_a = 99;       // pin associated with MOSFET, associated with return solenoid
    uint8_t _solenoid_pin_left_r = 99;        // pin associated with MOSFET, associated with actuation solenoid
    uint8_t _solenoid_pin_right_r = 99;       // pin associated with MOSFET, associated with return solenoid
    bool _increasing_pressure = false;     // used to track if pressure should be increasing
    bool _decreasing_pressure = false;     // used to track if pressure should be decreasing
    uint32_t _previous_millis = 0;    	   // will store last time solenoid was updated
} brakes;

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
	analogWrite( brakes._solenoid_pin_left_a, 0 );
    analogWrite( brakes._solenoid_pin_right_a, 0 );
    analogWrite( brakes._solenoid_pin_left_r, 0 );
    analogWrite( brakes._solenoid_pin_right_r, 0 );
}

// *****************************************************
// Function:    turn_on_brake_actuator_solenoids
// 
// Purpose:     Turn on brake actuator solenoids
// 
// Returns:     void
// 
// Parameters:  int16_t scalar to write to solenoids
// 
// *****************************************************
void turn_on_brake_actuator_solenoids( int16_t scaler )
{
	analogWrite( brakes._solenoid_pin_left_a, scaler );
    analogWrite( brakes._solenoid_pin_right_a, scaler );
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
	analogWrite( brakes._solenoid_pin_left_a, 0 );
    analogWrite( brakes._solenoid_pin_right_a, 0 );
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
void turn_on_brake_release_solenoids( int16_t scaler )
{
    analogWrite( brakes._solenoid_pin_left_r, scaler );
    analogWrite( brakes._solenoid_pin_right_r, scaler );	
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
	digitalWrite( brakes._solenoid_pin_left_r, LOW );
    digitalWrite( brakes._solenoid_pin_right_r, LOW );
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
    brakes._pressure_left = convert_to_voltage( analogRead( brakes._sensor_pin_left ) );
    brakes._pressure_right = convert_to_voltage( analogRead( brakes._sensor_pin_right ) );	
}

// *****************************************************
// Function:    init_brakes
// 
// Purpose:     Initializes the brakes
// 
// Returns:     void
// 
// Parameters:  uint8_t sensor_p_left
//				uint8_t sensor_p_right
//				uint8_t solenoid_pin_left_a
//				uint8_t solenoid_pin_right_a
//				uint8_t solenoid_pin_left_r
//				uint8_t solenoid_pin_right_r
// 
// *****************************************************
void init_brakes( uint8_t sensor_p_left,
				  uint8_t sensor_p_right,
				  uint8_t solenoid_pin_left_a,
				  uint8_t solenoid_pin_right_a,
				  uint8_t solenoid_pin_left_r,
				  uint8_t solenoid_pin_right_r )
{
	brakes._sensor_pin_left = sensor_p_left;
    brakes._sensor_pin_right = sensor_p_right;
    brakes._solenoid_pin_left_a = solenoid_pin_left_a;
    brakes._solenoid_pin_right_a = solenoid_pin_right_a;
    brakes._solenoid_pin_left_r = solenoid_pin_left_r;
    brakes._solenoid_pin_right_r = solenoid_pin_right_r;

    // initialize solenoid pins to off
    digitalWrite( brakes._solenoid_pin_left_a, LOW );
    digitalWrite( brakes._solenoid_pin_right_a, LOW );
    digitalWrite( brakes._solenoid_pin_left_r, LOW );
    digitalWrite( brakes._solenoid_pin_right_r, LOW );

    // set pinmode to OUTPUT
    pinMode( brakes._solenoid_pin_left_a, OUTPUT );
    pinMode( brakes._solenoid_pin_right_a, OUTPUT );
    pinMode( brakes._solenoid_pin_left_r, OUTPUT );
    pinMode( brakes._solenoid_pin_right_r, OUTPUT );
}

// *****************************************************
// Initialize Global Structs
// *****************************************************
//Accumulator accumulator( PIN_PACC, PIN_PUMP );
//accumulator_data_s accumulator = { PIN_PACC, PIN_PUMP };
//SMC smc( PIN_PMC1, PIN_PMC2, PIN_SMC );
//master_cylinder_solenoid_data_s master_cylinder_solonoid = { PIN_PMC1, PIN_PMC2, PIN_SMC };
//Brakes brakes = Brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR );
//brake_data_s brakes = { PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR }


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
    ps_ctrl_brake_report_msg * const data =
            ( ps_ctrl_brake_report_msg* ) tx_frame_ps_ctrl_brake_report.data;

    // set frame ID
    tx_frame_ps_ctrl_brake_report.id = ( uint32_t ) ( PS_CTRL_MSG_ID_BRAKE_REPORT );

    // set DLC
    tx_frame_ps_ctrl_brake_report.dlc = 8; //TODO

    // Set override flag
    data->override = local_override;

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
    // local vars
    uint32_t delta = 
    	timer_delta_ms( tx_frame_ps_ctrl_brake_report.timestamp );

    if( delta >= PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL )
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
    if( ( control_data->enabled == 1 ) &&
    	( control_enabled == false) )
    {
        control_enabled = true;
        brake_state_machine.transitionTo(Brake);
    }

    // disable control from the MKZ interface
    if( (control_data->enabled == 0) &&
    	( control_enabled == true) )
    {
        control_enabled = false;
        brake_state_machine.transitionTo(Wait);
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
    // brake pressure as reported from the C-CAN bus
    int16_t brake_pressure = chassis_data->brake_pressure;

    // take a reading from the brake pressure sensors
    update_brake_pressure( );

    // average the pressure of the rear and front lines
    float pressure = ( brakes._pressure_left + brakes._pressure_right ) / 2;
    DEBUG_PRINT( pressure );
    DEBUG_PRINT( "," );
    DEBUG_PRINT( brake_pressure );
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
    if( CAN.checkReceive() == CAN_MSGAVAIL )
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
        if( rx_frame.id == PS_CTRL_MSG_ID_BRAKE_COMMAND )
        {
            // process brake command
            process_ps_ctrl_brake_command(
            	( const ps_ctrl_brake_command_msg * const )rx_frame.data );
        }

        // check for a supported frame ID
        if( rx_frame.id == KIA_STATUS1_MESSAGE_ID )
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
    if( ( pressure_req > ZERO_PRESSURE + .01 ) &&
    	( control_enabled == true ) )
    {
        brake_state_machine.transitionTo( Brake );
    }
}


// *****************************************************
// Function:    wait_exit
// 
// Purpose:     Exit wait state
//				TODO: Currently does nothing
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

    digitalWrite( PIN_BRAKE_SWITCH_1, LOW );
    digitalWrite( PIN_BRAKE_SWITCH_2, LOW );

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
	int16_t delta_t = 10;
    int16_t curr_micros;
    int16_t last_micros = 0;

    double pressure_last;
    double pressure_rate_target;
    double pressure_rate;
    
    // maintain accumulator pressure
    maintain_accumulator_pressure( );

    // calculate a delta t
    last_micros = curr_micros;
    curr_micros = micros( );  // Fast loop, needs more precision than millis
    delta_t = curr_micros - last_micros;


    // take a reading from the brake pressure sensors
    update_brake_pressure( );
    pressure = ( brakes._pressure_left + brakes._pressure_right ) / 2;


    pressure_rate = ( pressure - pressure_last)/ delta_t;  // pressure/microsecond
    pressure_rate_target = pressure_req - pressure;

    pressure_last = pressure;

    pid_params.derivative_gain = 0.50;
    pid_params.proportional_gain = 10.0;
    pid_params.integral_gain = 1.5;

    int16_t ret = pid_update( &pid_params, pressure_rate_target - pressure_rate, 0.050 );

    if( ret == PID_SUCCESS )
    {
        double pressure_pid_output = pid_params.control;

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
        if( pressure_pid_output < -0.1 )
        {
            turn_off_brake_actuator_solenoids( );
            turn_on_brake_release_solenoids( calculate_solenoid_left_release_duty_cycle( pressure_pid_output ) );
        }

        // if pressure is too low
        if( pressure_pid_output > 0.1 )
        {
            turn_off_brake_release_solenoids( );
            turn_on_brake_actuator_solenoids( calculate_solenoid_left_actuation_duty_cycle( pressure_pid_output ) );
        }

        // if driver is not braking, transition to wait state
        if( pressure_req <= ZERO_PRESSURE )
        {
            DEBUG_PRINT( "pressure request below threshold" );
            brake_state_machine.transitionTo( Wait );
        }
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
    digitalWrite( PIN_BRAKE_SWITCH_1, HIGH );
    digitalWrite( PIN_BRAKE_SWITCH_2, HIGH );
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

    if( delta >= PS_CTRL_RX_WARN_TIMEOUT )
    {
        // disable control from the PolySync interface
        if( control_enabled )
        {
            Serial.println( "control disabled: timeout" );
            control_enabled = false;
            brake_state_machine.transitionTo( Wait );
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
//				Setup routine runs once you press reset
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void setup( void )
{
    // duty Scalers good for 0x05
    //solenoid_left_actuation_duty_max = 50;
    //solenoid_left_actuation_duty_min = 5;
    //solenoid_left_release_duty_max = 50;
    //solenoid_left_release_duty_min = 20;

    // duty Scalers good for 0x02
    solenoid_left_actuation_duty_max = 225;
    solenoid_left_actuation_duty_min = 100;
    solenoid_left_release_duty_max = 225;
    solenoid_left_release_duty_min = 100;

    // set the PWM timers, above the acoustic range
    TCCR3B = ( TCCR3B & 0xF8 ) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = ( TCCR4B & 0xF8 ) | 0x02; // pins 6,7,8 | timer 4

    /*
       0x01      31.374 KHz
       0x02      3.921 KHz
       0x03      980.3 Hz
       0x04      490.1 Hz
       0x05      245 hz
       0x06      122.5 hz
       0x07      30.63 hz
     */

    //Initialize structs
    init_accumulator( PIN_PACC, PIN_PUMP );
    init_master_cylinder_solenoid( PIN_PMC1, PIN_PMC2, PIN_SMC );
    init_brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR );

    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_brake_command, 0, sizeof( rx_frame_ps_ctrl_brake_command ) );

    // relay boards are active low, set to high before setting output to avoid unintended energisation of relay
    digitalWrite( PIN_BRAKE_SWITCH_1, HIGH );
    digitalWrite( PIN_BRAKE_SWITCH_2, HIGH );
    pinMode( PIN_BRAKE_SWITCH_1, OUTPUT );
    pinMode( PIN_BRAKE_SWITCH_2, OUTPUT );

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
// Purpose:     Update timestamp
//              Handle and publish CAN frames
//              Check for timeouts
//				Check for driver brake override
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void loop()
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
