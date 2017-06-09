/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"
#include "oscc_time.h"
#include "oscc_pid.h"
#include "oscc_signal_smoothing.h"

#include "globals.h"
#include "brake_control.h"
#include "master_cylinder.h"
#include "helper.h"


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
        || (g_brake_control_state.invalid_sensor_value == true) )
    {
        uint32_t current_time = GET_TIMESTAMP_MS();

        bool timeout = is_timeout(
            g_sensor_validity_last_check_timestamp,
            current_time,
            SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC );

        if( timeout == true )
        {
            g_sensor_validity_last_check_timestamp = current_time;


            bool master_cylinder_pressure_fault =
                check_master_cylinder_pressure_sensor_for_fault( );

            if( master_cylinder_pressure_fault == true )
            {
                DEBUG_PRINTLN( "Bad value read from master cylinder presure sensor" );
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

                g_brake_control_state.invalid_sensor_value = true;
            }
            else
            {
                g_brake_control_state.invalid_sensor_value = false;
            }
        }
    }
}


void read_pressure_sensor( void )
{
    int raw_pressure_left = analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
    int raw_pressure_right = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );

    float unfiltered_pressure_left = raw_adc_to_pressure( raw_pressure_left );
    float unfiltered_pressure_right = raw_adc_to_pressure( raw_pressure_right );

    const float filter_alpha = BRAKE_PRESSURE_SENSOR_EXPONENTIAL_FILTER_ALPHA;
    static float filtered_pressure_left = 0.0;
    static float filtered_pressure_right = 0.0;

    filtered_pressure_left = exponential_moving_average(
        filter_alpha,
        unfiltered_pressure_left,
        filtered_pressure_left);

    filtered_pressure_right = exponential_moving_average(
        filter_alpha,
        unfiltered_pressure_right,
        filtered_pressure_right);

    float pressure_average = ( filtered_pressure_left + filtered_pressure_right ) / 2.0;

    g_brake_control_state.current_sensor_brake_pressure = pressure_average;
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
    if ( g_brake_control_state.enabled == true )
    {
        static float pressure_target = 0.0;
        static float pressure = 0.0;

        static uint32_t control_loop_time = 0;

        uint32_t current_time = GET_TIMESTAMP_US();

        float loop_delta_t =
            (float) get_time_delta( control_loop_time, current_time );

        control_loop_time = current_time;

        loop_delta_t /= 1000.0;
        loop_delta_t /= 1000.0;

        read_pressure_sensor( );

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

        static interpolate_range_s pressure_ranges =
            { UINT16_MIN, UINT16_MAX, BRAKE_PRESSURE_MIN_IN_DECIBARS, BRAKE_PRESSURE_MAX_IN_DECIBARS };

        pressure = g_brake_control_state.current_sensor_brake_pressure;

        pressure_target = interpolate(
            g_brake_control_state.commanded_pedal_position,
            &pressure_ranges );

        int16_t ret = pid_update( &g_pid,
                                pressure_target,
                                pressure,
                                loop_delta_t );

        if ( ret == PID_SUCCESS )
        {
            float pid_output = g_pid.control;

            // pressure too high
            if ( pid_output < PID_OUTPUT_MIN )
            {
                static interpolate_range_s slr_ranges = {
                    PID_RELEASE_SOLENOID_CLAMPED_MIN,
                    PID_RELEASE_SOLENOID_CLAMPED_MAX,
                    RELEASE_SOLENOID_DUTY_CYCLE_MIN,
                    RELEASE_SOLENOID_DUTY_CYCLE_MAX };

                uint16_t slr_duty_cycle = 0;

                set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );

                pid_output = -pid_output;
                slr_duty_cycle = (uint16_t)interpolate( pid_output, &slr_ranges );

                if ( slr_duty_cycle > ( uint16_t )RELEASE_SOLENOID_DUTY_CYCLE_MAX )
                {
                    slr_duty_cycle = ( uint16_t )RELEASE_SOLENOID_DUTY_CYCLE_MAX;
                }

                set_release_solenoid_duty_cycle( slr_duty_cycle );

                if ( pressure_target < BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS )
                {
                    disable_brake_lights( );
                }
            }

            // pressure too low
            else if ( pid_output > PID_OUTPUT_MAX )
            {
                static interpolate_range_s sla_ranges = {
                    PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN,
                    PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX,
                    ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN,
                    ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX };

                uint16_t sla_duty_cycle = 0;

                enable_brake_lights( );

                set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );

                sla_duty_cycle = (uint16_t)interpolate( pid_output, &sla_ranges );

                if ( sla_duty_cycle > ( uint16_t )ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX )
                {
                    sla_duty_cycle = ( uint16_t )ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX;
                }

                set_accumulator_solenoid_duty_cycle( sla_duty_cycle );
            }

            // pressure within valid range
            else
            {
                if ( g_brake_control_state.commanded_pedal_position < BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS )
                {
                    disable_brake_lights( );
                }
            }
        }
    }
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
