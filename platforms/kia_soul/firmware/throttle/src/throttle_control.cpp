/**
 * @file throttle_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>
#include "debug.h"
#include "DAC_MCP49xx.h"
#include "oscc_dac.h"
#include "oscc_time.h"

#include "throttle_control.h"
#include "globals.h"


static void calculate_accelerator_spoof(
    const uint16_t accelerator_target,
    accelerator_position_s * const spoof );


void check_for_operator_override( void )
{
    if ( g_throttle_control_state.enabled == true
        || g_throttle_control_state.operator_override == true )
    {
        accelerator_position_s accelerator_position;

        read_accelerator_position_sensor( &accelerator_position );

        uint32_t accelerator_position_average =
            (accelerator_position.low + accelerator_position.high) / 2;

        if ( accelerator_position_average >= ACCELERATOR_OVERRIDE_THRESHOLD )
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


void check_for_sensor_faults( void )
{
    if ( (g_throttle_control_state.enabled == true)
        || (g_throttle_control_state.invalid_sensor_value == true) )
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

            int sensor_high = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH );
            int sensor_low = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW );

            // sensor pins tied to ground - a value of zero indicates disconnection
            if( (sensor_high == 0)
                || (sensor_low == 0) )
            {
                ++fault_count;

                if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
                {
                    disable_control( );

                    g_throttle_control_state.invalid_sensor_value = true;

                    DEBUG_PRINTLN( "Bad value read from accelerator position sensor" );
                }
            }
            else
            {
                g_throttle_control_state.invalid_sensor_value = false;
                fault_count = 0;
            }
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

        g_accelerator_spoof_output_sum =
            accelerator_spoof.high + accelerator_spoof.low;

        g_dac.outputA( accelerator_spoof.high );
        g_dac.outputB( accelerator_spoof.low );
    }
}


void enable_control( void )
{
    if( g_throttle_control_state.enabled == false
        && g_throttle_control_state.operator_override == false )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH,
            PIN_ACCELERATOR_POSITION_SENSOR_LOW );

        digitalWrite( PIN_SPOOF_ENABLE, HIGH );

        g_throttle_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void disable_control( void )
{
    if( g_throttle_control_state.enabled == true )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH,
            PIN_ACCELERATOR_POSITION_SENSOR_LOW );

        digitalWrite( PIN_SPOOF_ENABLE, LOW );

        g_throttle_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


void read_accelerator_position_sensor(
    accelerator_position_s * const value )
{
    value->high = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH ) << BIT_SHIFT_10BIT_TO_12BIT;
    value->low = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW ) << BIT_SHIFT_10BIT_TO_12BIT;
}


static void calculate_accelerator_spoof(
    const uint16_t accelerator_target,
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
