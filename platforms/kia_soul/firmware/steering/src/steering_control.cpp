#include "debug.h"

#include "globals.h"
#include "steering_control.h"


static int32_t get_analog_sample_average(
        int32_t num_samples,
        uint8_t pin );

static void write_sample_averages_to_dac(
        int16_t num_samples,
        uint8_t signal_pin_1,
        uint8_t signal_pin_2 );

bool check_driver_steering_override( )
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
    static const float torque_filter_alpha = 0.5;
    static const float steering_wheel_max_torque = 3000.0;

    static float filtered_torque_a = 0.0;
    static float filtered_torque_b = 0.0;

    bool override = false;

    float torque_sensor_a = ( float )( analogRead( PIN_TORQUE_POSITION_SENSOR_HIGH ) << 2 );
    float torque_sensor_b = ( float )( analogRead( PIN_TORQUE_POSITION_SENSOR_LOW ) << 2 );

    filtered_torque_a =
        ( torque_filter_alpha * torque_sensor_a ) +
            ( ( 1.0 - torque_filter_alpha ) * filtered_torque_a );

    filtered_torque_b =
        ( torque_filter_alpha * torque_sensor_b ) +
            ( ( 1.0 - torque_filter_alpha ) * filtered_torque_b );

    if ( ( abs( filtered_torque_a ) > steering_wheel_max_torque ) ||
         ( abs( filtered_torque_b ) > steering_wheel_max_torque ) )
    {
        override = true;
    }

    return ( override );
}


void calculate_torque_spoof( float torque, struct torque_spoof_t* spoof )
{
    spoof->low = 819.2 * ( 0.0008 * torque + 2.26 );
    spoof->high = 819.2 * ( -0.0008 * torque + 2.5 );
}


void enable_control( )
{
    // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac(
        num_samples,
        PIN_TORQUE_POSITION_SENSOR_HIGH,
        PIN_TORQUE_POSITION_SENSOR_LOW );

    // Enable the signal interrupt relays
    digitalWrite( PIN_SPOOF_ENABLE, HIGH );

    control_state.enabled = true;

    DEBUG_PRINTLN( "Control enabled" );
}


void disable_control( )
{
    // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac(
        num_samples,
        PIN_TORQUE_POSITION_SENSOR_HIGH,
        PIN_TORQUE_POSITION_SENSOR_LOW );

    // Disable the signal interrupt relays
    digitalWrite( PIN_SPOOF_ENABLE, LOW );

    control_state.enabled =false;

    DEBUG_PRINTLN( "Control disabled" );
}


static int32_t get_analog_sample_average(
    int32_t num_samples,
    uint8_t pin )
{
    int32_t sum = 0;
    int32_t i = 0;

    for ( i = 0; i < num_samples; ++i )
    {
        sum += analogRead( pin );
    }

    return ( (sum / num_samples) << 2 );
}


static void write_sample_averages_to_dac(
        int16_t num_samples,
        uint8_t signal_pin_1,
        uint8_t signal_pin_2 )
{
    int32_t averages[ 2 ] = { 0, 0 };

    averages[0] = get_analog_sample_average( num_samples, signal_pin_1);
    averages[1] = get_analog_sample_average( num_samples, signal_pin_2);

    // Write measured values to DAC to avoid a signal discontinuity when the SCM takes over
    dac.outputA( averages[0] );
    dac.outputB( averages[1] );
}
