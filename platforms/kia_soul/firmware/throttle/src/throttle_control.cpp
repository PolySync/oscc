/**
 * @file throttle_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "DAC_MCP49xx.h"
#include "oscc_dac.h"

#include "throttle_control.h"
#include "globals.h"


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

        if ( accelerator_position_normalized >= ACCELERATOR_OVERRIDE_THRESHOLD )
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
        write_sample_averages_to_dac(
            g_dac,
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
        write_sample_averages_to_dac(
            g_dac,
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
    value->low =
        analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW ) << BIT_SHIFT_10BIT_TO_12BIT;

    value->high =
        analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH ) << BIT_SHIFT_10BIT_TO_12BIT;
}


static void calculate_accelerator_spoof(
    const float accelerator_target,
    accelerator_position_s * const spoof )
{
    if ( spoof != NULL )
    {
        uint16_t spoof_low =
            STEPS_PER_VOLT
            * ((SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * accelerator_target)
            + SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET);

        uint16_t spoof_high =
            STEPS_PER_VOLT
            * ((SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * accelerator_target)
            + SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET);

        spoof->low =
            constrain(
                spoof_low,
                SPOOF_LOW_SIGNAL_RANGE_MIN,
                SPOOF_LOW_SIGNAL_RANGE_MAX );

        spoof->high =
            constrain(
                spoof_high,
                SPOOF_HIGH_SIGNAL_RANGE_MIN,
                SPOOF_HIGH_SIGNAL_RANGE_MAX );
    }
}
