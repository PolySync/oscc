/**
 * @file steering_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "pid.h"
#include "dac.h"

#include "globals.h"
#include "steering_control.h"


static void calculate_torque_spoof(
    const float torque_target,
    steering_torque_s * const spoof );

static void read_torque_sensor(
    steering_torque_s * value );


void check_for_operator_override( void )
{
    if( g_steering_control_state.enabled == true )
    {
        // The parameters below; torque_filter_alpha and steering_wheel_max_torque,
        // can be used to modify how selective the steering override functionality
        // is. If torque_filter_alpha or steering_wheel_max_torque is increased
        // then steering override will be more selective about disabling on driver
        // input. That is, it will require a harder input for the steering wheel
        // to automatically disable. If these values are lowered then the steering
        // override will be less selective; this may result in drastic movements
        // of the joystick controller triggering steering override.
        // It is expected behavior that if a user uses the joystick controller to
        // purposefully "fight" the direction of steering wheel movement that this
        // will cause a steering override with the below parameters. That is if
        // the steering wheel is drastically "jerked" back and forth, opposing the
        // direction of steering wheel movement and purposefully trying to cause
        // an unstable situation, the steering override is expected to be
        // triggered.

        // This function checks the voltage input from the steering
        // wheel's torque sensors to determine if the driver is attempting
        // to steer the vehicle.  This must be done over time by taking
        // periodic samples of the input torque voltage, calculating the
        // difference between the two and then passing that difference
        // through a basic exponential filter to smooth the input.

        // The required response time for the filter is 250 ms, which at
        // 50ms per sample is 5 samples.  As such, the alpha for the
        // exponential filter is 0.5 to make the input go "close to" zero
        // in 5 samples.

        // The implementation is:
        //     s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )

        // If the filtered torque exceeds the max torque, it is an
        // indicator that there is feedback on the steering wheel and the
        // control should be disabled.

        // The final check determines if the a and b signals are opposite
        // each other.  If they are not, it is an indicator that there is
        // a problem with one of the sensors.  The check is looking for a
        // 90% tolerance.

        static const float torque_filter_alpha = 0.5;

        steering_torque_s torque;

        read_torque_sensor( &torque );

        static float filtered_torque_a =
            ( torque_filter_alpha * torque.high ) +
                ( ( 1.0 - torque_filter_alpha ) * filtered_torque_a );

        static float filtered_torque_b =
            ( torque_filter_alpha * torque.low ) +
                ( ( 1.0 - torque_filter_alpha ) * filtered_torque_b );

        if ( (abs(filtered_torque_a) > OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC)
            || (abs(filtered_torque_b) > OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC) )
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
        float time_between_loops_in_sec = 0.05;

        // Calculate steering angle rates (millidegrees/microsecond)
        float steering_angle_rate =
            ( g_steering_control_state.steering_angle
            - g_steering_control_state.steering_angle_last )
            / time_between_loops_in_sec;

        float steering_angle_rate_target =
            ( g_steering_control_state.commanded_steering_angle
            - g_steering_control_state.steering_angle )
            / time_between_loops_in_sec;

        // Save the angle for next iteration
        g_steering_control_state.steering_angle_last =
            g_steering_control_state.steering_angle;

        steering_angle_rate_target =
            constrain( steering_angle_rate_target,
                    STEERING_ANGLE_RATE_MIN_IN_DEGREES_PER_USEC,
                    STEERING_ANGLE_RATE_MAX_IN_DEGREES_PER_USEC );

        g_pid.proportional_gain = PID_PROPORTIONAL_GAIN;
        g_pid.integral_gain = PID_INTEGRAL_GAIN;
        g_pid.derivative_gain = PID_DERIVATIVE_GAIN;

        pid_update(
                &g_pid,
                steering_angle_rate_target,
                steering_angle_rate,
                time_between_loops_in_sec );

        float control = g_pid.control;

        control = constrain( control,
                            TORQUE_MIN_IN_NEWTON_METERS,
                            TORQUE_MAX_IN_NEWTON_METERS );

        steering_torque_s torque_spoof;

        calculate_torque_spoof( control, &torque_spoof );

        g_torque_sum = (uint8_t) ( torque_spoof.low + torque_spoof.high );

        g_dac.outputA( torque_spoof.low );
        g_dac.outputB( torque_spoof.high );
    }
}


void enable_control( void )
{
    if( g_steering_control_state.enabled == false )
    {
        // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
        // signal discontinuity when the SCM takes over
        static uint16_t num_samples = 20;
        write_sample_averages_to_dac(
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
        // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
        // signal discontinuity when the SCM takes over
        static uint16_t num_samples = 20;
        write_sample_averages_to_dac(
            g_dac,
            num_samples,
            PIN_TORQUE_SENSOR_HIGH,
            PIN_TORQUE_SENSOR_LOW );

        // Disable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, LOW );

        g_steering_control_state.enabled =false;

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
    const float torque_target,
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
