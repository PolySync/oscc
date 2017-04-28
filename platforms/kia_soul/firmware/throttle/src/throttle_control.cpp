/**
 * @file throttle_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "DAC_MCP49xx.h"

#include "throttle_control.h"
#include "globals.h"


static int32_t get_analog_sample_average(
    const int32_t num_samples,
    const uint8_t pin );

static void write_sample_averages_to_g_dac(
    const int16_t num_samples,
    const uint8_t signal_pin_1,
    const uint8_t signal_pin_2 );

static void calculate_accelerator_spoof(
    const float accelerator_target,
    accelerator_position_s * const spoof );


void check_for_operator_override( void )
{
    if ( g_throttle_control_state.enabled == true )
    {
        accelerator_position_s accelerator_position;

        read_accelerator_position_sensor( &accelerator_position );

        uint32_t accelerator_position_normalized =
            (accelerator_position.low + accelerator_position.high) / 2;

        if ( accelerator_position_normalized > PARAM_ACCELERATOR_OVERRIDE_THRESHOLD_IN_DECIBARS )
        {
            disable_control( );

            g_throttle_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_throttle_control_state.operator_override = false;
        }
    }
}


void update_throttle( void )
{
    if ( g_throttle_control_state.enabled == true )
    {
        accelerator_position_s accelerator_spoof;

        calculate_accelerator_spoof(
                g_throttle_control_state.commanded_accelerator_position,
                &accelerator_spoof );

        g_dac.outputA( accelerator_spoof.high );
        g_dac.outputB( accelerator_spoof.low );
    }
}


void enable_control( void )
{
    if( g_throttle_control_state.enabled == false )
    {
        // Sample the current values, smooth them, and write measured accelerator position values to DAC to avoid a
        // signal discontinuity when the SCM takes over
        static uint16_t num_samples = 20;
        write_sample_averages_to_g_dac(
            num_samples,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH,
            PIN_ACCELERATOR_POSITION_SENSOR_LOW );

        // Enable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, HIGH );

        g_throttle_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void disable_control( void )
{
    if( g_throttle_control_state.enabled == true )
    {
        // Sample the current values, smooth them, and write measured accelerator position values to DAC to avoid a
        // signal discontinuity when the SCM relinquishes control
        static uint16_t num_samples = 20;
        write_sample_averages_to_g_dac(
            num_samples,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH,
            PIN_ACCELERATOR_POSITION_SENSOR_LOW );

        // Disable the signal interrupt relays
        digitalWrite( PIN_SPOOF_ENABLE, LOW );

        g_throttle_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


void read_accelerator_position_sensor(
    accelerator_position_s * const value )
{
    // shifting required to go from 10 bit to 12 bit
    value->low = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW ) << 2;
    value->high = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH ) << 2;
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


static void calculate_accelerator_spoof(
    const float accelerator_target,
    accelerator_position_s * const spoof )
{
    if ( spoof != NULL )
    {
        // values calculated with min/max calibration curve and tuned for neutral
        // balance.  DAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
        spoof->low = 819.2 * ( 0.0004 * accelerator_target + 0.366 );
        spoof->high = 819.2 * ( 0.0008 * accelerator_target + 0.732 );

        // range = 300 - ~1800
        spoof->low = constrain( spoof->low, 0, 1800 );
        // range = 600 - ~3500
        spoof->high = constrain( spoof->high, 0, 3500 );
    }
}
