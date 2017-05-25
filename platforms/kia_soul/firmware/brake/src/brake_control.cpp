/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"
#include "oscc_time.h"
#include "oscc_pid.h"

#include "globals.h"
#include "brake_control.h"
#include "master_cylinder.h"
#include "helper.h"


void brake_lights_off( void )
{
    digitalWrite( PIN_BRAKE_LIGHT, LOW );
}


void brake_lights_on( void )
{
    digitalWrite( PIN_BRAKE_LIGHT, HIGH );
}


void brake_command_accumulator_solenoids( const uint16_t duty_cycle )
{
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT, duty_cycle );
}


void brake_command_release_solenoids( const uint16_t duty_cycle )
{
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_LEFT, duty_cycle );
    analogWrite( PIN_RELEASE_SOLENOID_FRONT_RIGHT, duty_cycle );
}


void brake_enable( void )
{
    if ( g_brake_control_state.enabled == false
         && g_brake_control_state.operator_override == false )
    {
        master_cylinder_close( );
        brake_command_release_solenoids( SOLENOID_PWM_OFF );
        g_brake_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void brake_disable( void )
{
    if ( g_brake_control_state.enabled == true )
    {
        brake_command_accumulator_solenoids( SOLENOID_PWM_OFF );

        brake_command_release_solenoids( SOLENOID_PWM_ON );

        brake_lights_off( );

        delay( 15 );

        master_cylinder_open( );

        brake_command_release_solenoids( SOLENOID_PWM_OFF );

        g_brake_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


void check_for_operator_override( void )
{
    if( g_brake_control_state.enabled == true
        || g_brake_control_state.operator_override == true )
    {
        // This function checks the voltage input from the brake pedal
        // sensors to determine if the driver is attempting to brake
        // the vehicle.  This must be done over time by taking
        // periodic samples of the input voltage, calculating the
        // difference between the two and then passing that difference
        // through a basic exponential filter to smooth the input.

        // The required response time for the filter is 250 ms, which at
        // 50ms per sample is 5 samples.  As such, the alpha for the
        // exponential filter is 0.5 to make the input go "close to" zero
        // in 5 samples.

        // The implementation is:
        //     s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )

        // If the filtered input exceeds the max voltage, it is an
        // indicator that the driver is pressing on the brake pedal
        // and the control should be disabled.
        static const float filter_alpha = 0.05;

        static float filtered_input_1 = 0.0;
        static float filtered_input_2 = 0.0;

        float sensor_1 = ( float )( analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 ) );
        float sensor_2 = ( float )( analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 ) );

        sensor_1 = raw_adc_to_pressure( ( uint16_t )sensor_1 );
        sensor_2 = raw_adc_to_pressure( ( uint16_t )sensor_2 );

        filtered_input_1 = ( filter_alpha * sensor_1 ) +
            ( ( 1.0 - filter_alpha ) * filtered_input_1 );

        filtered_input_2 = ( filter_alpha * sensor_2 ) +
            ( ( 1.0 - filter_alpha ) * filtered_input_2 );

        if ( ( filtered_input_1 >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ) ||
            ( filtered_input_2 >= DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ) )
        {
            brake_disable( );

            g_brake_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_brake_control_state.operator_override = false;
        }
    }
}


void brake_update_pressure( void )
{
    uint16_t raw_left_pressure = analogRead( PIN_PRESSURE_SENSOR_FRONT_LEFT );
    uint16_t raw_right_pressure = analogRead( PIN_PRESSURE_SENSOR_FRONT_RIGHT );

    float pressure_left = raw_adc_to_pressure( raw_left_pressure );
    float pressure_right = raw_adc_to_pressure( raw_right_pressure );

    g_brake_control_state.current_sensor_brake_pressure =
        (pressure_left + pressure_right) / 2.0;
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

    brake_command_release_solenoids( SOLENOID_PWM_OFF );
    brake_command_accumulator_solenoids( SOLENOID_PWM_OFF );

    brake_lights_off( );
    pinMode( PIN_BRAKE_LIGHT, OUTPUT );
}


void brake_update( void )
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

        brake_update_pressure( );

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

        // Requested pressure
        DEBUG_PRINT(pressure_target);

        // Pressure at wheels (PFR and PFL)
        DEBUG_PRINT(",");
        DEBUG_PRINT(pressure);

        // PID output
        DEBUG_PRINT(",");
        DEBUG_PRINT( g_pid.control );

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

                brake_command_accumulator_solenoids( SOLENOID_PWM_OFF );

                pid_output = -pid_output;
                slr_duty_cycle = (uint16_t)interpolate( pid_output, &slr_ranges );

                if ( slr_duty_cycle > ( uint16_t )RELEASE_SOLENOID_DUTY_CYCLE_MAX )
                {
                    slr_duty_cycle = ( uint16_t )RELEASE_SOLENOID_DUTY_CYCLE_MAX;
                }

                brake_command_release_solenoids( slr_duty_cycle );

                DEBUG_PRINT(",0,");
                DEBUG_PRINT(slr_duty_cycle);

                if ( pressure_target < BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS )
                {
                    brake_lights_off( );
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

                brake_lights_on( );

                brake_command_release_solenoids( SOLENOID_PWM_OFF );

                sla_duty_cycle = (uint16_t)interpolate( pid_output, &sla_ranges );

                if ( sla_duty_cycle > ( uint16_t )ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX )
                {
                    sla_duty_cycle = ( uint16_t )ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX;
                }

                brake_command_accumulator_solenoids( sla_duty_cycle );

                DEBUG_PRINT(",");
                DEBUG_PRINT(sla_duty_cycle);
                DEBUG_PRINT(",0");
            }

            // pressure within valid range
            else
            {
                if ( g_brake_control_state.commanded_pedal_position < BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS )
                {
                    brake_lights_off( );
                }
            }
        }

        DEBUG_PRINTLN("");
    }
}
