#include <SPI.h>
#include "arduino_init.h"
#include "mcp_can.h"
#include "chassis_state_can_protocol.h"
#include "brake_can_protocol.h"
#include "oscc_pid.h"
#include "oscc_serial.h"
#include "oscc_can.h"
#include "oscc_time.h"
#include "debug.h"

#include "globals.h"
#include "accumulator.h"
#include "helper.h"
#include "master_cylinder.h"
#include "brake_control.h"
#include "communications.h"
#include "init.h"


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

    int16_t operator_override;

    int16_t can_pressure;
    uint16_t pedal_command;
};


struct accumulator_data_s accumulator =
{
    0.0     // pressure
};


struct master_cylinder_data_s master_cylinder =
{
    0.0,    // pressure1
    0.0     // pressure2
};


struct pressure_at_tires_data_s pressure_at_tires =
{
    0.0,    // pressure_left
    0.0     // pressure_right
};


struct brake_data_s brakes =
{
    0.0,                        // pressure
    false,                      // enable
    false,                      // enable_request
    0,                          // rx_timestamp
    0,                          // operator_override
    0,                          // can_pressure
    0                           // pedal_command
};


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
static float raw_adc_to_voltage( int16_t input )
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
static void print_pressure_info()
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
static void accumulator_read_pressure( )
{
    int16_t raw_accumulator_data = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );

    float pressure = raw_adc_to_voltage( raw_accumulator_data );

    accumulator.pressure = pressure;
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
static void master_cylinder_read_pressure( )
{
    int16_t raw_smc1_data = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 );
    int16_t raw_smc2_data = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 );

    float pressure_smc1 = raw_adc_to_voltage( raw_smc1_data );
    float pressure_smc2 = raw_adc_to_voltage( raw_smc2_data );

    master_cylinder.pressure1 = pressure_smc1;
    master_cylinder.pressure2 = pressure_smc2;
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
static void process_serial_byte( uint8_t incoming_byte )
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

            brake_command_accumulator_solenoids( 255 );

            DEBUG_PRINTLN("opened SLAs");

            break;

        case 'k':

            brake_command_accumulator_solenoids( 0 );

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
static void process_serial()
{
    uint8_t incomingSerialByte;

    // read and parse incoming serial commands
    if( Serial.available() > 0 )
    {
        incomingSerialByte = Serial.read();

        process_serial_byte( incomingSerialByte );
    }
}


int main( void )
{
    init_arduino( );

    init_globals( );

    init_devices( );

    init_communication_interfaces( );

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        check_for_incoming_message( );

        publish_reports( );

        process_serial( );

        brake_update_pressure( );

        accumulator_read_pressure( );

        master_cylinder_read_pressure( );

        print_pressure_info( );
    }
}
