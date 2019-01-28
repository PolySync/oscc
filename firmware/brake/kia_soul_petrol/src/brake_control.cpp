/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>

#include "accumulator.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "dtc.h"
#include "globals.h"
#include "helper.h"
#include "master_cylinder.h"
#include "oscc_pid.h"
#include "vehicles.h"
#include "oscc_check.h"

/*
 * @brief Number of consecutive faults that can occur when reading the
 *        sensors before control is disabled.
 *
 */
#define SENSOR_VALIDITY_CHECK_FAULT_COUNT ( 4 )


static float read_pressure_sensor( void );
static void disable_brake_lights( void );
static void enable_brake_lights( void );
static bool check_master_cylinder_pressure_sensor_for_fault( void );
static bool check_accumulator_pressure_sensor_for_fault( void );
static bool check_wheel_pressure_sensor_for_fault( void );
static void startup_check( void );
static void pressure_startup_check( void );
static void pump_startup_check( void );


void set_accumulator_solenoid_duty_cycle( const uint16_t duty_cycle )
{
    cli();
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, duty_cycle );
    sei();
}


void set_release_solenoid_duty_cycle( const uint16_t duty_cycle )
{
    cli();
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_RIGHT, duty_cycle );
    sei();
}


void enable_control( void )
{
    if ( (g_brake_control_state.enabled == false)
         && (g_brake_control_state.operator_override == false)
         && (g_brake_control_state.startup_pressure_check_error == false)
         && (g_brake_control_state.startup_pump_motor_check_error== false) )
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
    static condition_state_s operator_override_state = CONDITION_STATE_INIT;

    if( g_brake_control_state.enabled == true
        || g_brake_control_state.operator_override == true )
    {
        master_cylinder_pressure_s master_cylinder_pressure;

        master_cylinder_read_pressure( &master_cylinder_pressure );

        bool override_detected =
            ( master_cylinder_pressure.sensor_1_pressure >= BRAKE_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ) ||
            ( master_cylinder_pressure.sensor_2_pressure >= BRAKE_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS );

        bool operator_overridden = condition_exceeded_duration(
                override_detected,
                FAULT_HYSTERESIS,
                &operator_override_state);

        if ( operator_overridden == true )
        {
            disable_control( );

            DTC_SET(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_OPERATOR_OVERRIDE );

            publish_fault_report( );

            g_brake_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            DTC_CLEAR(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_OPERATOR_OVERRIDE );

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

            DTC_SET(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );

            publish_fault_report( );
        }
        else
        {
            DTC_CLEAR(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );
        }
    }
}


void brake_init( void )
{
    cli();
    digitalWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, LOW );
    digitalWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, LOW );
    digitalWrite( PIN_RELEASE_SOLENOID_FRONT_LEFT, LOW );
    digitalWrite( PIN_RELEASE_SOLENOID_FRONT_RIGHT, LOW );
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_1, LOW );
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_2, LOW );
    sei();

    pinMode( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, OUTPUT );
    pinMode( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, OUTPUT );
    pinMode( PIN_RELEASE_SOLENOID_FRONT_LEFT, OUTPUT );
    pinMode( PIN_RELEASE_SOLENOID_FRONT_RIGHT, OUTPUT );
    pinMode( PIN_WHEEL_PRESSURE_CHECK_1, OUTPUT );
    pinMode( PIN_WHEEL_PRESSURE_CHECK_2, OUTPUT );

    set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );
    set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

    disable_brake_lights( );
    pinMode( PIN_BRAKE_LIGHT, OUTPUT );

#ifdef BRAKE_STARTUP_TEST
    startup_check( );
#endif
}


void update_brake( void )
{
    if ( g_brake_control_state.enabled == true )
    {
        cli();

        static float pressure_at_wheels_target = 0.0;
        static float pressure_at_wheels_current = 0.0;

        static uint32_t control_loop_time = 0;

        uint32_t current_time = micros();

        float time_between_loops = current_time - control_loop_time;

        control_loop_time = current_time;

        // Division by 1000 twice overcomes the Arduino's mathematic limitations
        // and allows for a conversion from microseconds to seconds
        time_between_loops /= 1000.0;
        time_between_loops /= 1000.0;

        static interpolate_range_s pressure_ranges =
            { 0.0, 1.0, BRAKE_PRESSURE_MIN_IN_DECIBARS, BRAKE_PRESSURE_MAX_IN_DECIBARS };

        pressure_at_wheels_target = interpolate(
            g_brake_control_state.commanded_pedal_position,
            &pressure_ranges );

        pressure_at_wheels_current = read_pressure_sensor( );

        int16_t ret = pid_update(
            &g_pid,
            pressure_at_wheels_target,
            pressure_at_wheels_current,
            time_between_loops );

        if ( ret == PID_SUCCESS )
        {
            float pid_output = g_pid.control;

            // pressure too high
            if ( pid_output < BRAKE_PID_OUTPUT_MIN )
            {
                static interpolate_range_s slr_ranges = {
                    BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MIN,
                    BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MAX,
                    BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MIN,
                    BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MAX };

                uint16_t slr_duty_cycle = 0;

                set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

                pid_output = -pid_output;
                slr_duty_cycle = (uint16_t)interpolate( pid_output, &slr_ranges );

                if ( slr_duty_cycle > ( uint16_t )BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MAX )
                {
                    slr_duty_cycle = ( uint16_t )BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MAX;
                }

                set_release_solenoid_duty_cycle( slr_duty_cycle );

                if ( pressure_at_wheels_target < BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS )
                {
                    disable_brake_lights( );
                }
            }

            // pressure too low
            else if ( pid_output > BRAKE_PID_OUTPUT_MAX )
            {
                static interpolate_range_s sla_ranges = {
                    BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN,
                    BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX,
                    BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN,
                    BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX };

                uint16_t sla_duty_cycle = 0;

                enable_brake_lights( );

                set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );

                sla_duty_cycle = (uint16_t)interpolate( pid_output, &sla_ranges );

                if ( sla_duty_cycle > ( uint16_t )BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX )
                {
                    sla_duty_cycle = ( uint16_t )BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX;
                }

                set_accumulator_solenoid_duty_cycle( sla_duty_cycle );
            }
        }

        sei();
    }
}


static float read_pressure_sensor( void )
{
    cli();
    int raw_adc_front_left =
        analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );

    int raw_adc_front_right =
        analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );
    sei();

    float pressure_front_left = raw_adc_to_pressure( raw_adc_front_left );
    float pressure_front_right = raw_adc_to_pressure( raw_adc_front_right );

    return ( (pressure_front_left + pressure_front_right) / 2.0 );
}


static void disable_brake_lights( void )
{
    cli();
    digitalWrite( PIN_BRAKE_LIGHT, LOW );
    sei();
}


static void enable_brake_lights( void )
{
    cli();
    digitalWrite( PIN_BRAKE_LIGHT, HIGH );
    sei();
}


static bool check_master_cylinder_pressure_sensor_for_fault( void )
{
    bool fault_occurred = false;

    static int fault_count = 0;

    cli();
    int master_cylinder_pressure_1 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 );
    int master_cylinder_pressure_2 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 );
    sei();

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

    cli();
    int accumulator_pressure = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );
    sei();

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

    cli();
    int wheel_pressure_front_left =  analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
    int wheel_pressure_front_right = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );
    sei();

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


static void startup_check( void )
{
    pressure_startup_check( );
    pump_startup_check( );
}


static void pressure_startup_check( void )
{
    cli();
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_1, HIGH );
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_2, HIGH );
    sei();

    delay(250);

    cli();
    int pressure_front_left = analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
    int pressure_front_right = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );
    int pressure_accumulator = analogRead( PIN_ACCUMULATOR_PRESSURE_SENSOR );
    sei();

    if( (pressure_front_left < BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN)
        || (pressure_front_left > BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MAX)
        || (pressure_front_right < BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN)
        || (pressure_front_right > BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MAX) )
    {
        g_brake_control_state.startup_pressure_check_error = true;

        DEBUG_PRINTLN( "Startup pressure check error" );
    }
    else
    {
        g_brake_control_state.startup_pressure_check_error = false;
    }

    cli();
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_1, LOW );
    digitalWrite( PIN_WHEEL_PRESSURE_CHECK_2, LOW );
    sei();

    delay(250);
}


static void pump_startup_check( void )
{
    accumulator_turn_pump_on();
    delay(250);

    cli();
    int motor_check = analogRead( PIN_ACCUMULATOR_PUMP_MOTOR_CHECK );
    sei();

    // should not be 0 if the pump is on
    if( motor_check == 0 )
    {
        g_brake_control_state.startup_pump_motor_check_error = true;

        DEBUG_PRINTLN( "Startup pump motor error" );
    }
    else
    {
        g_brake_control_state.startup_pump_motor_check_error = false;
    }

    accumulator_turn_pump_off();
}
