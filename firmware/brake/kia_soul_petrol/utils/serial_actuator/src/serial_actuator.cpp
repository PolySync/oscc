#include <SPI.h>
#include "arduino_init.h"
#include "mcp_can.h"
#include "can_protocols/brake_can_protocol.h"
#include "oscc_pid.h"
#include "oscc_serial.h"
#include "oscc_can.h"
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


master_cylinder_pressure_s master_cylinder =
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
    DEBUG_PRINT( master_cylinder.sensor_1_pressure );

    DEBUG_PRINT( ",PMC2," );
    DEBUG_PRINTLN( master_cylinder.sensor_2_pressure );
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

            set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_ON );

            DEBUG_PRINTLN("opened SLAs");

            break;

        case 'k':

            set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

            DEBUG_PRINTLN("closed SLAs");

            break;

        case 'm':

            set_release_solenoid_duty_cycle( SOLENOID_PWM_ON );

            DEBUG_PRINTLN("opened SLRs");

            break;

        case ',':

            set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );

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

    digitalWrite(PIN_WHEEL_PRESSURE_CHECK_1, LOW);
    digitalWrite(PIN_WHEEL_PRESSURE_CHECK_1, LOW);
    pinMode(PIN_WHEEL_PRESSURE_CHECK_1, OUTPUT);
    pinMode(PIN_WHEEL_PRESSURE_CHECK_1, OUTPUT);

    DEBUG_PRINTLN( "initialization complete" );

    while( true )
    {
        check_for_incoming_message( );

        publish_brake_report( );

        process_serial( );

        int pressure_at_tires_pressure_left_raw = analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
        int pressure_at_tires_pressure_right_raw = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );
        pressure_at_tires.pressure_left = raw_adc_to_pressure(pressure_at_tires_pressure_left_raw);
        pressure_at_tires.pressure_right = raw_adc_to_pressure(pressure_at_tires_pressure_right_raw);

        int accumulator_pressure_raw = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );
        accumulator.pressure = raw_adc_to_pressure(accumulator_pressure_raw);

        int master_cylinder_sensor_1_pressure_raw = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 );
        int master_cylinder_sensor_2_pressure_raw = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 );
        master_cylinder.sensor_1_pressure = raw_adc_to_pressure(master_cylinder_sensor_1_pressure_raw);
        master_cylinder.sensor_2_pressure = raw_adc_to_pressure(master_cylinder_sensor_2_pressure_raw);

        print_pressure_info( );
    }
}
