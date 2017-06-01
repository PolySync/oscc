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
#include "oscc_signal_smoothing.h"

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
    steering_torque_s unfiltered_torque;

    unfiltered_torque.high = analogRead( PIN_TORQUE_SENSOR_HIGH ) << BIT_SHIFT_10BIT_TO_12BIT;
    unfiltered_torque.low = analogRead( PIN_TORQUE_SENSOR_LOW ) << BIT_SHIFT_10BIT_TO_12BIT;

    const float filter_alpha = TORQUE_SENSOR_EXPONENTIAL_FILTER_ALPHA;
    static float filtered_torque_high = 0.0;
    static float filtered_torque_low = 0.0;

    filtered_torque_high = exponential_moving_average(
        filter_alpha,
        unfiltered_torque.high,
        filtered_torque_high);

    filtered_torque_low = exponential_moving_average(
        filter_alpha,
        unfiltered_torque.low,
        filtered_torque_low);

    value->high = filtered_torque_high;
    value->low = filtered_torque_low;
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
