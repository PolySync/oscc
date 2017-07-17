/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"
#include "oscc_pid.h"
#include "dtc.h"
#include "brake_can_protocol.h"
#include "kia_soul.h"

#include "globals.h"
#include "brake_control.h"
#include "communications.h"
#include "master_cylinder.h"
#include "helper.h"


/*
 * @brief Number of consecutive faults that can occur when reading the
 *        sensors before control is disabled.
 *
 */
#define SENSOR_VALIDITY_CHECK_FAULT_COUNT ( 4 )


static void disable_brake_lights( void );
static void enable_brake_lights( void );
static bool check_master_cylinder_pressure_sensor_for_fault( void );
static bool check_accumulator_pressure_sensor_for_fault( void );
static bool check_wheel_pressure_sensor_for_fault( void );


void set_accumulator_solenoid_duty_cycle( const uint16_t duty_cycle )
{
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, duty_cycle );
}


void set_release_solenoid_duty_cycle( const uint16_t duty_cycle )
{
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_RIGHT, duty_cycle );
}


void enable_control( void )
{
    if ( g_brake_control_state.enabled == false
         && g_brake_control_state.operator_override == false )
    {
        master_cylinder_close( );

        set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );

        g_brake_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void disable_control( void )
{
    if ( g_brake_control_state.enabled == true )
    {
        set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

        set_release_solenoid_duty_cycle( SOLENOID_PWM_ON );

        disable_brake_lights( );

        delay( 15 );

        master_cylinder_open( );

        set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );

        g_brake_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


void check_for_operator_override( void )
{
    if( g_brake_control_state.enabled == true
        || g_brake_control_state.operator_override == true )
    {
        master_cylinder_pressure_s master_cylinder_pressure;

        master_cylinder_read_pressure( &master_cylinder_pressure );

        if ( ( master_cylinder_pressure.sensor_1_pressure >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ) ||
            ( master_cylinder_pressure.sensor_2_pressure >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ) )
        {
            disable_control( );

            publish_fault_report( );

            g_brake_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_brake_control_state.operator_override = false;
        }
    }
}


void check_for_sensor_faults( void )
{
    if ( (g_brake_control_state.enabled == true)
        || DTC_CHECK(g_brake_control_state.dtcs, OSCC_BRAKE_DTC_INVALID_SENSOR_VAL) )
    {
        bool master_cylinder_pressure_fault =
            check_master_cylinder_pressure_sensor_for_fault( );

        if( master_cylinder_pressure_fault == true )
        {
            DEBUG_PRINTLN( "Bad value read from master cylinder pressure sensor" );
        }


        bool accumulator_pressure_fault =
            check_accumulator_pressure_sensor_for_fault( );

        if( accumulator_pressure_fault == true )
        {
            DEBUG_PRINTLN( "Bad value read from accumulator pressure sensor" );
        }


        bool wheel_pressure_fault =
            check_wheel_pressure_sensor_for_fault( );

        if( wheel_pressure_fault == true )
        {
            DEBUG_PRINTLN( "Bad value read from wheel pressure sensor" );
        }


        if( (master_cylinder_pressure_fault == true)
            || (accumulator_pressure_fault == true)
            || (wheel_pressure_fault == true) )
        {
            disable_control( );

            publish_fault_report( );

            DTC_SET(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );
        }
        else
        {
            DTC_CLEAR(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );
        }
    }
}


void read_pressure_sensor( void )
{
    g_brake_control_state.brake_pressure_front_left =
        analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );

    g_brake_control_state.brake_pressure_front_right =
        analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );
}


void brake_init( void )
{
    digitalWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, LOW );
    digitalWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, LOW );
    digitalWrite( PIN_RELEASE_SOLENOID_FRONT_LEFT, LOW );
    digitalWrite( PIN_RELEASE_SOLENOID_FRONT_RIGHT, LOW );

    pinMode( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, OUTPUT );
    pinMode( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, OUTPUT );
    pinMode( PIN_RELEASE_SOLENOID_FRONT_LEFT, OUTPUT );
    pinMode( PIN_RELEASE_SOLENOID_FRONT_RIGHT, OUTPUT );

    set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );
    set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

    disable_brake_lights( );
    pinMode( PIN_BRAKE_LIGHT, OUTPUT );
}


void update_brake( void )
{
    read_pressure_sensor( );
}


static void disable_brake_lights( void )
{
    digitalWrite( PIN_BRAKE_LIGHT, LOW );
}


static void enable_brake_lights( void )
{
    digitalWrite( PIN_BRAKE_LIGHT, HIGH );
}


static bool check_master_cylinder_pressure_sensor_for_fault( void )
{
    bool fault_occurred = false;

    static int fault_count = 0;

    int master_cylinder_pressure_1 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 );
    int master_cylinder_pressure_2 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 );

    // sensor pins tied to ground - a value of zero indicates disconnection
    if( (master_cylinder_pressure_1 == 0)
        || (master_cylinder_pressure_2 == 0) )
    {
        ++fault_count;

        if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
        {
            fault_occurred = true;
        }
    }
    else
    {
        fault_count = 0;
    }

    return fault_occurred;
}


static bool check_accumulator_pressure_sensor_for_fault( void )
{
    bool fault_occurred = false;

    static int fault_count = 0;

    int accumulator_pressure = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );

    // sensor pins tied to ground - a value of zero indicates disconnection
    if( accumulator_pressure == 0 )
    {
        ++fault_count;

        if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
        {
            fault_occurred = true;
        }
    }
    else
    {
        fault_count = 0;
    }

    return fault_occurred;
}


static bool check_wheel_pressure_sensor_for_fault( void )
{
    bool fault_occurred = false;

    static int fault_count = 0;

    int wheel_pressure_front_left =  analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
    int wheel_pressure_front_right = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );

    // sensor pins tied to ground - a value of zero indicates disconnection
    if( (wheel_pressure_front_left == 0)
        || (wheel_pressure_front_right == 0) )
    {
        ++fault_count;

        if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
        {
            fault_occurred = true;
        }
    }
    else
    {
        fault_count = 0;
    }

    return fault_occurred;
}
