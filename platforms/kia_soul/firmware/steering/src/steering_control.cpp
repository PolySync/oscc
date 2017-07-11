/**
 * @file steering_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include <stdlib.h>
#include "debug.h"
#include "oscc_pid.h"
#include "oscc_dac.h"
#include "oscc_time.h"

#include "globals.h"
#include "steering_control.h"


#define MSEC_TO_SEC(msec) ( (msec) / 1000.0 )


static void calculate_torque_spoof(
    const int16_t torque_target,
    steering_torque_s * const spoof );

static void read_torque_sensor(
    steering_torque_s * value );


void check_for_operator_override( void )
{
    if( g_steering_control_state.enabled == true
        || g_steering_control_state.operator_override == true )
    {
        steering_torque_s torque;

        read_torque_sensor( &torque );

        if ( (abs(torque.high) >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC)
            || (abs(torque.low) >= OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC) )
        {
            disable_control( );

            g_steering_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_steering_control_state.operator_override = false;
        }
    }
}


void check_for_sensor_faults( void )
{
    if ( (g_steering_control_state.enabled == true)
        || (g_steering_control_state.invalid_sensor_value == true) )
    {
        uint32_t current_time = GET_TIMESTAMP_MS();

        bool timeout = is_timeout(
            g_sensor_validity_last_check_timestamp,
            current_time,
            SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC );

        static int fault_count = 0;

        if( timeout == true )
        {
            g_sensor_validity_last_check_timestamp = current_time;

            int sensor_high = analogRead( PIN_TORQUE_SENSOR_HIGH );
            int sensor_low = analogRead( PIN_TORQUE_SENSOR_LOW );

            // sensor pins tied to ground - a value of zero indicates disconnection
            if( (sensor_high == 0)
                || (sensor_low == 0) )
            {
                ++fault_count;

                if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
                {
                    disable_control( );

                    g_steering_control_state.invalid_sensor_value = true;

                    DEBUG_PRINTLN( "Bad value read from torque sensor" );
                }
            }
            else
            {
                g_steering_control_state.invalid_sensor_value = false;
                fault_count = 0;
            }
        }
    }
}


void update_steering( void )
{
    if (g_steering_control_state.enabled == true )
    {
        float time_between_loops_in_sec =
            MSEC_TO_SEC( CONTROL_LOOP_INTERVAL_IN_MSEC );

        // Calculate steering angle rates (millidegrees/microsecond)
        float steering_wheel_angle_rate =
            ( g_steering_control_state.current_steering_wheel_angle
            - g_steering_control_state.previous_steering_wheel_angle )
            / time_between_loops_in_sec;

        float steering_wheel_angle_rate_target =
            ( g_steering_control_state.commanded_steering_wheel_angle
            - g_steering_control_state.current_steering_wheel_angle )
            / time_between_loops_in_sec;

        // Save the angle for next iteration
        g_steering_control_state.previous_steering_wheel_angle =
            g_steering_control_state.current_steering_wheel_angle;

        steering_wheel_angle_rate_target =
            constrain( steering_wheel_angle_rate_target,
                       -g_steering_control_state.commanded_steering_wheel_angle_rate,
                       g_steering_control_state.commanded_steering_wheel_angle_rate );

        pid_update(
                &g_pid,
                steering_wheel_angle_rate_target,
                steering_wheel_angle_rate,
                time_between_loops_in_sec );

        float control = g_pid.control;

        control = constrain( control,
                             TORQUE_MIN_IN_NEWTON_METERS,
                             TORQUE_MAX_IN_NEWTON_METERS );

        steering_torque_s torque_spoof;

        calculate_torque_spoof( control, &torque_spoof );

        g_spoofed_torque_output_sum = torque_spoof.low + torque_spoof.high;

        g_dac.outputA( torque_spoof.low );
        g_dac.outputB( torque_spoof.high );
    }
}


void enable_control( void )
{
    if( g_steering_control_state.enabled == false
        && g_steering_control_state.operator_override == false )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_TORQUE_SENSOR_HIGH,
            PIN_TORQUE_SENSOR_LOW );

        // Enable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, HIGH );

        g_steering_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void disable_control( void )
{
    if( g_steering_control_state.enabled == true )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_TORQUE_SENSOR_HIGH,
            PIN_TORQUE_SENSOR_LOW );

        // Disable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, LOW );

        g_steering_control_state.enabled = false;

        pid_zeroize( &g_pid, PID_WINDUP_GUARD );

        DEBUG_PRINTLN( "Control disabled" );
    }
}

static void read_torque_sensor(
    steering_torque_s * value )
{
    value->high = analogRead( PIN_TORQUE_SENSOR_HIGH ) << BIT_SHIFT_10BIT_TO_12BIT;
    value->low = analogRead( PIN_TORQUE_SENSOR_LOW ) << BIT_SHIFT_10BIT_TO_12BIT;
}


static void calculate_torque_spoof(
    const int16_t torque_target,
    steering_torque_s * const spoof )
{
    if( spoof != NULL )
    {
        spoof->low =
            STEPS_PER_VOLT
            * ((SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALAR * torque_target)
            + SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET);

        spoof->high =
            STEPS_PER_VOLT
            * ((SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALAR * torque_target)
            + SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET);
    }
}
