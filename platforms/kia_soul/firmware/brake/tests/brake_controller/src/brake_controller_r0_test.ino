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
#include <FiniteStateMachine.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "common.h"
#include "control_protocol_can.h"
#include "PID.h"




// *****************************************************
// static global data
// *****************************************************


#define PSYNC_DEBUG_FLAG

// show us if debugging
#ifdef PSYNC_DEBUG_FLAG
    #warning "PSYNC_DEBUG_FLAG defined"
    #define DEBUG_PRINTLN(x) Serial.println(x)
    #define DEBUG_PRINT(x) Serial.print(x)
#else
    #define DEBUG_PRINTLN(x)
    #define DEBUG_PRINT(x)
#endif




// *****************************************************
// Constants
// *****************************************************

const float MIN_ACCUMULATOR_PRESSURE = 2.1;              // min accumulator pressure to maintain
const float MAX_ACCUMULATOR_PRESSURE = 2.3;              // max accumulator pressure to maintain
const float PEDAL_THRESHOLD = 0.6;       // Pressure for pedal interference




// *****************************************************
// static global data/macros
// *****************************************************

#define CAN_SHIELD_CHIP_SELECT_PIN (53)

// ms
#define PS_CTRL_RX_WARN_TIMEOUT (150)

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


// pressure at tires structure
struct pressure_at_tires_data_s
{
    float pressure_left;
    float pressure_right;
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
MCP_CAN CAN(CAN_SHIELD_CHIP_SELECT_PIN); // Set CS pin for the CAN shield


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
// Left and Right wheel pressure data values
// *****************************************************

struct pressure_at_tires_data_s pressure_at_tires =
{
    0.0,    // pressure_left
    0.0     // pressure_right
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
// Function:    raw_adc_to_voltage
//
// Purpose:     Convert the raw ADC reading (0 - 1023)
//              to a pressure (0 - 5V)
//
// Returns:     float - pressure
//
// Parameters:  input - raw ADC reading
//
// *****************************************************
float raw_adc_to_voltage( int16_t input )
{
    float voltage = ( ( float )input * ( 5.0 / 1023.0 ) );
    return voltage;
}


// *****************************************************
// Function:    print_pressure_info
//
// Purpose:     Print pressure being read from sensors
//  and CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void print_pressure_info()
{
    DEBUG_PRINT( "CAN," );
    DEBUG_PRINT( brakes.can_pressure );

    DEBUG_PRINT( ",PACC," );
    DEBUG_PRINT( accumulator.pressure );

    DEBUG_PRINT( ",PFL," );
    DEBUG_PRINT( pressure_at_tires.pressure_left );

    DEBUG_PRINT( ",PFR," );
    DEBUG_PRINT( pressure_at_tires.pressure_right );

    DEBUG_PRINT( ",PMC1," );
    DEBUG_PRINT( master_cylinder.pressure1 );

    DEBUG_PRINT( ",PMC2," );
    DEBUG_PRINTLN( master_cylinder.pressure2 );
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
void accumulator_read_pressure( )
{
    int16_t raw_accumulator_data = analogRead( PIN_PACC );

    float pressure = raw_adc_to_voltage( raw_accumulator_data );

    accumulator.pressure = pressure;
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
void master_cylinder_read_pressure( )
{
    int16_t raw_smc1_data = analogRead( PIN_PMC1 );
    int16_t raw_smc2_data = analogRead( PIN_PMC2 );

    float pressure_smc1 = raw_adc_to_voltage( raw_smc1_data );
    float pressure_smc2 = raw_adc_to_voltage( raw_smc2_data );

    master_cylinder.pressure1 = pressure_smc1;
    master_cylinder.pressure2 = pressure_smc2;
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
// Parameters:  int16_t - duty_cycle - value to send to the PWM
//
// *****************************************************
void brake_command_actuator_solenoids( int16_t duty_cycle )
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
// Parameters:  int16_t - duty_cycle - value to send to the PWM
//
// *****************************************************
void brake_command_release_solenoids( int16_t duty_cycle )
{
    analogWrite( PIN_SLRFL, duty_cycle );
    analogWrite( PIN_SLRFR, duty_cycle );
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

    float pressure_left = raw_adc_to_voltage( raw_left_pressure );
    float pressure_right = raw_adc_to_voltage( raw_right_pressure );

    pressure_at_tires.pressure_left = pressure_left;
    pressure_at_tires.pressure_right = pressure_right;

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
    DEBUG_PRINTLN( brakes.pedal_command );
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
// Function:    process_serial_byte
//
// Purpose:     Process test commands from user.
//
// Returns:     void
//
// Parameters:  incoming_byte - byte received from serial
// connection
//
// *****************************************************
void process_serial_byte( uint8_t incoming_byte )
{
    switch( incoming_byte )
    {
        case 'u':

            master_cylinder_open();

            DEBUG_PRINTLN("opened SMCs");

            break;

        case 'i':

            master_cylinder_close( );

            DEBUG_PRINTLN("closed SMCs");

            break;

        case 'j':

            brake_command_actuator_solenoids( 255 );

            DEBUG_PRINTLN("opened SLAs");

            break;

        case 'k':

            brake_command_actuator_solenoids( 0 );

            DEBUG_PRINTLN("closed SLAs");

            break;

        case 'm':

            brake_command_release_solenoids( 255 );

            DEBUG_PRINTLN("opened SLRs");

            break;

        case ',':

            brake_command_release_solenoids( 0 );

            DEBUG_PRINTLN("closed SLRs");

            break;

        case 'p':

            accumulator_turn_pump_on();

            DEBUG_PRINTLN("pump on");

            break;

        case '[':

            accumulator_turn_pump_off();

            DEBUG_PRINTLN("pump off");

            break;
    }
}


// *****************************************************
// Function:    process_serial
//
// Purpose:     Process incoming serial byte
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void process_serial()
{
    uint8_t incomingSerialByte;

    // read and parse incoming serial commands
    if( Serial.available() > 0 )
    {
        incomingSerialByte = Serial.read();

        process_serial_byte( incomingSerialByte );
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

    process_serial( );

    brake_update_pressure( );

    accumulator_read_pressure( );

    master_cylinder_read_pressure( );

    print_pressure_info( );
}


