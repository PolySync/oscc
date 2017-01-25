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
int calculate_SLA_duty_cycle( float pre )
{
  int scaled = abs( pre ) * 512;
  int scale =  map( scaled, 0, 1024, SLA_duty_min, SLA_duty_max );
  return scale;
}


int calculate_SLR_duty_cycle( float pre )
{
  int scaled = abs( pre ) * 512;
  int scale =  map( scaled, 0, 1024, SLR_duty_min, SLR_duty_max );
  return scale;
}

float pressure_to_voltage( int MPa )
{
    return ( MPa + 217.1319446 ) / 505.5662053;
    // convert MPa pressure to equivalent voltage
    return MPa;
}

int voltage_to_pressure( float voltage )
{
    // convert voltage reading from sensors to pressure in MPa
    return ( voltage * 505.5662053 ) - 217.1319446;
}

// convert the ADC reading (which goes from 0 - 1023) to a voltage (0 - 5V):
float convert_to_voltage( int input )
{
    return input * ( 5.0 / 1023.0 );
}


// MOSFET pin (digital) definitions ( MOSFETs control the solenoids )
// pins are not perfectly sequential because the clock frequency of certain pins is different.
// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
const byte PIN_SLAFL = 3;      // front left actuation
const byte PIN_SLAFR = 5;      // front right actuation
// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
const byte PIN_SLRFL = 6;      // front left return
const byte PIN_SLRFR = 7;      // front right return
const byte PIN_SMC = 8;      // master cylinder solenoids (two of them)

const byte PIN_PUMP = 9;     // accumulator pump motor


// brake spoofer relay pin definitions
const byte PIN_BRAKE_SWITCH_1 = 48;
const byte PIN_BRAKE_SWITCH_2 = 49;


// sensor pin (analog) definitions
const byte PIN_PACC = 9;       // pressure accumulator sensor
const byte PIN_PMC1 = 10;      // pressure master cylinder sensor 1
const byte PIN_PMC2 = 11;      // pressure master cylinder sensor 2
const byte PIN_PRL = 12;       // pressure rear left sensor
const byte PIN_PFR = 13;       // pressure front right sensor
const byte PIN_PFL = 14;       // pressure front left sensor
const byte PIN_PRR = 15;       // pressure rear right sensor


// the following are guesses, these need to be debugged/researched
const double ZERO_PRESSURE = 0.48;        // The voltage the sensors read when no pressure is present
const double PRESSURE_STEP = 0.2;         // The amount that the 'a' and 'd' commands change the
                                          // voltage each time they are pressed.
const double MIN_PACC = 2.3;              // minumum accumulator pressure to maintain
const double MAX_PACC = 2.4;              // max accumulator pressure to maintain
const double PEDAL_THRESH = 0.5;          // Pressure for pedal interference

int SLA_duty_max,
    SLA_duty_min,
    SLR_duty_max,
    SLR_duty_min;

double pressure_req,
       pressure;

uint8_t incoming_serial_byte;

unsigned long previous_millis = 0;

bool control_enabled = false;
int local_override = 0;

// initialize states
State Wait = State( wait_enter, wait_update, wait_exit );        // Wait for brake instructions
State Brake = State( brake_enter, brake_update, brake_exit );    // Control braking


// initialize state machine, start in state: Wait
FSM brakeStateMachine = FSM( Wait );


// *****************************************************
// local data structures
// *****************************************************
// accumulator structure
struct Accumulator {
    float _pressure = 0.0;    // pressure is initliazed at 0
    byte _sensor_pin = 99;     // set to 99 to avoid and accidental assignments
    byte _control_pin = 99;
    Accumulator( byte sensor_p, byte relay_p );

    void updatePressure( )
    {
    }

    // turn relay on or off
    void pumpOn( )
    {
      digitalWrite( _control_pin, HIGH );
    }

    void pumpOff( )
    {
      digitalWrite( _control_pin, LOW );
    }

    // maintain accumulator pressure
    void maintainPressure( )
    {
      _pressure = convert_to_voltage( analogRead( _sensor_pin ) );

      if( _pressure < MIN_PACC )
      {
          pumpOn( );
      }

      if( _pressure > MAX_PACC )
      {
          pumpOff( );
      }
    }
};


// accumulator constructor
Accumulator::Accumulator( byte sensor_pin, byte control_pin )
{
  _sensor_pin = sensor_pin;
  _control_pin = control_pin;

  pinMode( _control_pin, OUTPUT ); // set pinmode to OUTPUT

  // initialize pump to off
  pumpOff( );
}


// master Solenoid structure
struct SMC {
    float _pressure1 = 0.0; // Initialize pressures to 0.0 to avoid false values
    float _pressure2 = 0.0;
    byte _sensor1_pin = 99;
    byte _sensor2_pin = 99;
    byte _control_pin = 99;

    SMC( byte sensor1_pin, byte sensor2_pin, byte control_pin );

    void checkPedal()
    {
        // read pressures at sensors
        _pressure1 = convert_to_voltage( analogRead( _sensor1_pin ) );
        _pressure2 = convert_to_voltage( analogRead( _sensor2_pin ) );

        // if current pedal pressure is greater than limit (because of
        // driver override by pressing the brake pedal), disable.
        if ( ( _pressure1 > PEDAL_THRESH ) ||
        	 ( _pressure2 > PEDAL_THRESH ) )
        {
            DEBUG_PRINT( "Brake Pedal Detected" );
            pressure_req = ZERO_PRESSURE;
            local_override = 1;
            brakeStateMachine.transitionTo( Wait );
        }
        else
        {
            local_override = 0;
        }
    }

    void solenoidsClose( )
    {
        analogWrite( _control_pin, 255 );
    }

    void solenoidsOpen( )
    {
        analogWrite( _control_pin, 0 );
    }
};


SMC::SMC( byte sensor1_pin, byte sensor2_pin, byte control_pin )
{
  _sensor1_pin = sensor1_pin;
  _sensor2_pin = sensor2_pin;
  _control_pin = control_pin;

  pinMode( _control_pin, OUTPUT );  // We're writing to pin, set as an output

  solenoidsOpen( );
}


// wheel structure
struct Brakes {
    float _pressure_left = 0.0;            // last known right-side pressure
    float _pressure_right = 0.0;           // last known left-side pressure
    byte _sensor_pin_left = 99;            // pin associated with left-side  pressure sensor
    byte _sensor_pin_right = 99;           // pin associated with right-side pressure sensors
    byte _solenoid_pin_left_a = 99;        // pin associated with MOSFET, associated with actuation solenoid
    byte _solenoid_pin_right_a = 99;       // pin associated with MOSFET, associated with return solenoid
    byte _solenoid_pin_left_r = 99;        // pin associated with MOSFET, associated with actuation solenoid
    byte _solenoid_pin_right_r = 99;       // pin associated with MOSFET, associated with return solenoid
    bool _increasing_pressure = false;     // used to track if pressure should be increasing
    bool _decreasing_pressure = false;     // used to track if pressure should be decreasing
    unsigned long _previous_millis = 0;    // will store last time solenoid was updated

    Brakes( byte sensor_pin_left,
    		byte sensor_pin_right,
    		byte solenoid_pin_left_a,
    		byte solenoid_pin_right_a,
    		byte solenoid_pin_left_r,
    		byte solenoid_pin_right_r );

    void depowerSolenoids( )
    {
      analogWrite( _solenoid_pin_left_a, 0 );
      analogWrite( _solenoid_pin_right_a, 0 );
      analogWrite( _solenoid_pin_left_r, 0 );
      analogWrite( _solenoid_pin_right_r, 0 );
    }

    // fill pressure
    void powerSLA( int scaler )
    {
        analogWrite( _solenoid_pin_left_a, scaler );
        analogWrite( _solenoid_pin_right_a, scaler );
    }

    void depowerSLA( )
    {
        analogWrite( _solenoid_pin_left_a, 0 );
        analogWrite( _solenoid_pin_right_a, 0 );
    }

    // spill pressure
    void powerSLR( int scaler )
    {
        analogWrite( _solenoid_pin_left_r, scaler );
        analogWrite( _solenoid_pin_right_r, scaler );
    }

    void depowerSLR( )
    {
        digitalWrite( _solenoid_pin_left_r, LOW );
        digitalWrite( _solenoid_pin_right_r, LOW );
    }

    // take a pressure reading
    void updatePressure( )
    {
      _pressure_left = convert_to_voltage( analogRead( _sensor_pin_left ) );
      _pressure_right = convert_to_voltage( analogRead( _sensor_pin_right ) );
    }
};

// brake constructor
Brakes::Brakes( byte sensor_p_left,
				byte sensor_p_right,
				byte solenoid_pin_left_a,
				byte solenoid_pin_right_a,
				byte solenoid_pin_left_r,
				byte solenoid_pin_right_r ) {

  _sensor_pin_left = sensor_p_left;
  _sensor_pin_right = sensor_p_right;
  _solenoid_pin_left_a = solenoid_pin_left_a;
  _solenoid_pin_right_a = solenoid_pin_right_a;
  _solenoid_pin_left_r = solenoid_pin_left_r;
  _solenoid_pin_right_r = solenoid_pin_right_r;

  // initialize solenoid pins to off
  digitalWrite( _solenoid_pin_left_a, LOW );
  digitalWrite( _solenoid_pin_right_a, LOW );
  digitalWrite( _solenoid_pin_left_r, LOW );
  digitalWrite( _solenoid_pin_right_r, LOW );

  // set pinmode to OUTPUT
  pinMode( _solenoid_pin_left_a, OUTPUT );
  pinMode( _solenoid_pin_right_a, OUTPUT );
  pinMode( _solenoid_pin_left_r, OUTPUT );
  pinMode( _solenoid_pin_right_r, OUTPUT );
}


// Instantiate objects
Accumulator accumulator( PIN_PACC, PIN_PUMP );
SMC smc( PIN_PMC1, PIN_PMC2, PIN_SMC );
Brakes brakes = Brakes( PIN_PFL, PIN_PFR, PIN_SLAFL, PIN_SLAFR, PIN_SLRFL, PIN_SLRFR );


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
        brakeStateMachine.transitionTo(Brake);
    }

    // disable control from the MKZ interface
    if( (control_data->enabled == 0) &&
    	( control_enabled == true) )
    {
        control_enabled = false;
        brakeStateMachine.transitionTo(Wait);
    }

    rx_frame_ps_ctrl_brake_command.timestamp = millis( );

    unsigned int pedal_command = control_data->pedal_command;
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
    int brake_pressure = chassis_data->brake_pressure;

    // take a reading from the brake pressure sensors
    brakes.updatePressure( );

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
    smc.solenoidsOpen();
    brakes.depowerSLA();
    brakes.depowerSLR();

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
    accumulator.maintainPressure( );

    // TODO: Is this check needed? Don't we force transition elsewhere?
    if( ( pressure_req > ZERO_PRESSURE + .01 ) &&
    	( control_enabled == true ) )
    {
        brakeStateMachine.transitionTo( Brake );
    }
}


// *****************************************************
// Function:    wait_exit
// 
// Purpose:     Exit wait state
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
    smc.solenoidsClose( );

    digitalWrite( PIN_BRAKE_SWITCH_1, LOW );
    digitalWrite( PIN_BRAKE_SWITCH_2, LOW );

    // close SLRRs, they are normally open for failsafe conditions
    brakes.depowerSLR( );

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
	int delta_t = 10,
    curr_micros,
    last_micros = 0;

    double pressure_last,
    	   pressure_rate_target,
    	   pressure_rate;
    
    // maintain accumulator pressure
    accumulator.maintainPressure( );

    // calculate a delta t
    last_micros = curr_micros;
    curr_micros = micros( );  // Fast loop, needs more precision than millis
    delta_t = curr_micros - last_micros;


    // take a reading from the brake pressure sensors
    brakes.updatePressure( );
    pressure = ( brakes._pressure_left + brakes._pressure_right ) / 2;


    pressure_rate = ( pressure - pressure_last)/ delta_t;  // pressure/microsecond
    pressure_rate_target = pressure_req - pressure;

    pressure_last = pressure;

    pid_params.derivative_gain = 0.50;
    pid_params.proportional_gain = 10.0;
    pid_params.integral_gain = 1.5;

    int ret = pid_update( &pid_params, pressure_rate_target - pressure_rate, 0.050 );

    if( ret == PID_SUCCESS )
    {
        double pressure_pid_output = pid_params.control;

        // constrain to min/max
        pressure_pid_output = m_constrain(
                (float) (pressure_pid_output),
                (float) -2.0f,
                (float) 2.0f );

        // some logic to set a samplerate for data which is sent to processing for plotting
        unsigned long current_millis = millis();
        if ((unsigned long)(current_millis - previous_millis) >= 100)
        {
            previous_millis = current_millis;
        }

        // if pressure is too high
        if( pressure_pid_output < -0.1 )
        {
            brakes.depowerSLA( );
            brakes.powerSLR( calculate_SLR_duty_cycle( pressure_pid_output ) );
        }

        // if pressure is too low
        if( pressure_pid_output > 0.1 )
        {
            brakes.depowerSLR( );
            brakes.powerSLA( calculate_SLA_duty_cycle( pressure_pid_output ) );
        }

        // if driver is not braking, transition to wait state
        if( pressure_req <= ZERO_PRESSURE )
        {
            DEBUG_PRINT( "pressure request below threshold" );
            brakeStateMachine.transitionTo( Wait );
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
    smc.solenoidsOpen( );

    // depower wheel solenoids to vent brake pressure at wheels
    brakes.depowerSLA( );

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
            brakeStateMachine.transitionTo( Wait );
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
    //SLA_duty_max = 50;
    //SLA_duty_min = 5;
    //SLR_duty_max = 50;
    //SLR_duty_min = 20;

    // duty Scalers good for 0x02
    SLA_duty_max = 225;
    SLA_duty_min = 100;
    SLR_duty_max = 225;
    SLR_duty_min = 100;

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

    // zero
    last_update_ms = 0;
    memset( &rx_frame_ps_ctrl_brake_command, 0, sizeof( rx_frame_ps_ctrl_brake_command ) );

    // relay boards are active low, set to high before setting output to avoid unintended energisation of relay
    digitalWrite( PIN_BRAKE_SWITCH_1, HIGH );
    digitalWrite( PIN_BRAKE_SWITCH_2, HIGH );
    pinMode( PIN_BRAKE_SWITCH_1, OUTPUT );
    pinMode( PIN_BRAKE_SWITCH_2, OUTPUT );

    // depower all the things
    accumulator.pumpOff( );
    smc.solenoidsOpen( );

    // close rear slrs. These should open only for emergencies and to release brake pressure
    brakes.depowerSLR( );
    brakes.depowerSLA( );

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

void loop()
{

    // update the global system update timestamp, ms
    last_update_ms = millis( );

    handle_ready_rx_frames( );

    publish_timed_tx_frames( );

    check_rx_timeouts( );

    // check pressures on master cylinder (pressure from pedal)
    smc.checkPedal( );

    brakeStateMachine.update( );
}
