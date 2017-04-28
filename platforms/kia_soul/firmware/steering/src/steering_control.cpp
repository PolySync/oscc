/**
 * @file steering_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "pid.h"

#include "globals.h"
#include "steering_control.h"


static int32_t get_analog_sample_average(
    const int32_t num_samples,
    const uint8_t pin );

static void write_sample_averages_to_g_dac(
    const int16_t num_samples,
    const uint8_t signal_pin_1,
    const uint8_t signal_pin_2 );

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
        static const float steering_wheel_max_torque = 3000.0;

        steering_torque_s torque;

        read_torque_sensor( &torque );

        static float filtered_torque_a =
            ( torque_filter_alpha * torque.high ) +
                ( ( 1.0 - torque_filter_alpha ) * filtered_torque_a );

        static float filtered_torque_b =
            ( torque_filter_alpha * torque.low ) +
                ( ( 1.0 - torque_filter_alpha ) * filtered_torque_b );

        if ( (abs(filtered_torque_a) > steering_wheel_max_torque)
            || (abs(filtered_torque_b) > steering_wheel_max_torque) )
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
        // Calculate steering angle rates (degrees/microsecond)
        float steering_angle_rate =
            ( g_steering_control_state.steering_angle -
            g_steering_control_state.steering_angle_last ) / 0.05;

        float steering_angle_rate_target =
            ( g_steering_control_state.commanded_steering_angle -
            g_steering_control_state.steering_angle ) / 0.05;

        // Save the angle for next iteration
        g_steering_control_state.steering_angle_last =
            g_steering_control_state.steering_angle;

        steering_angle_rate_target =
            constrain( steering_angle_rate_target,
                    ( float )-PARAM_STEERING_ANGLE_RATE_MAX_IN_DEGREES_PER_USEC,
                    ( float )PARAM_STEERING_ANGLE_RATE_MAX_IN_DEGREES_PER_USEC );

        g_pid.proportional_gain = PARAM_PID_PROPORTIONAL_GAIN;
        g_pid.integral_gain = PARAM_PID_INTEGRAL_GAIN;
        g_pid.derivative_gain = PARAM_PID_DERIVATIVE_GAIN;

        pid_update(
                &g_pid,
                steering_angle_rate_target,
                steering_angle_rate,
                0.050 );

        float control = g_pid.control;

        control = constrain( control,
                            -1500.0f,
                            1500.0f );

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
        write_sample_averages_to_g_dac(
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
        write_sample_averages_to_g_dac(
            num_samples,
            PIN_TORQUE_SENSOR_HIGH,
            PIN_TORQUE_SENSOR_LOW );

        // Disable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, LOW );

        g_steering_control_state.enabled =false;

        pid_zeroize( &g_pid, PARAM_PID_WINDUP_GUARD );

        DEBUG_PRINTLN( "Control disabled" );
    }
}

static void read_torque_sensor(
    steering_torque_s * value )
{
    // shifting required to go from 10 bit to 12 bit
    value->high = analogRead( PIN_TORQUE_SENSOR_HIGH ) << 2;
    value->low = analogRead( PIN_TORQUE_SENSOR_LOW ) << 2;
}


static int32_t get_analog_sample_average(
    const int32_t num_samples,
    const uint8_t pin )
{
    int32_t sum = 0;
    int32_t i = 0;

    for ( i = 0; i < num_samples; ++i )
    {
        sum += analogRead( pin );
    }

    return ( (sum / num_samples) << 2 );
}


static void write_sample_averages_to_g_dac(
    const int16_t num_samples,
    const uint8_t signal_pin_1,
    const uint8_t signal_pin_2 )
{
    int32_t averages[ 2 ] = { 0, 0 };

    averages[0] = get_analog_sample_average( num_samples, signal_pin_1);
    averages[1] = get_analog_sample_average( num_samples, signal_pin_2);

    // Write measured values to DAC to avoid a signal discontinuity when the SCM takes over
    g_dac.outputA( averages[0] );
    g_dac.outputB( averages[1] );
}


static void calculate_torque_spoof(
    const float torque_target,
    steering_torque_s * const spoof )
{
    if( spoof != NULL )
    {
        spoof->low = 819.2 * ( 0.0008 * torque_target + 2.26 );
        spoof->high = 819.2 * ( -0.0008 * torque_target + 2.5 );
    }
}
