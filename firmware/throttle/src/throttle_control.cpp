/**
 * @file throttle_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>

#include "can_protocols/throttle_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "dtc.h"
#include "globals.h"
#include "oscc_dac.h"
#include "throttle_control.h"
#include "vehicles.h"


static void read_accelerator_position_sensor(
    accelerator_position_s * const value );


static bool check_voltage_grounded( uint16_t high, uint16_t low );


void check_for_faults( void )
{
    accelerator_position_s accelerator_position;

    if ( ( g_throttle_control_state.enabled == true )
        || (g_throttle_control_state.dtcs > 0) )
    {
        read_accelerator_position_sensor( &accelerator_position );

        uint32_t accelerator_position_average =
            (accelerator_position.low + accelerator_position.high) / 2;

        // sensor pins tied to ground - a value of zero indicates disconnection
        if( check_voltage_grounded( accelerator_position.high,
                                    accelerator_position.low ) )
        {
            disable_control( );

            DTC_SET(
                g_throttle_control_state.dtcs,
                OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL );

            publish_fault_report( );

            DEBUG_PRINTLN( "Bad value read from accelerator position sensor" );
        }
        else if ( accelerator_position_average >= ACCELERATOR_OVERRIDE_THRESHOLD
          && g_throttle_control_state.operator_override == false )
        {
            disable_control( );

            DTC_SET(
                g_throttle_control_state.dtcs,
                OSCC_THROTTLE_DTC_OPERATOR_OVERRIDE );

            publish_fault_report( );

            g_throttle_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_throttle_control_state.dtcs = 0;

            if ( g_throttle_control_state.operator_override == true )
            {
                g_throttle_control_state.operator_override = false;
            }
        }
    }
}


void update_throttle(
    uint16_t spoof_command_high,
    uint16_t spoof_command_low )
{
    if ( g_throttle_control_state.enabled == true )
    {
        uint16_t spoof_high =
            constrain(
                spoof_command_high,
                THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN,
                THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX );

        uint16_t spoof_low =
            constrain(
                spoof_command_low,
                THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN,
                THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX );

        cli();
        g_dac.outputA( spoof_high );
        g_dac.outputB( spoof_low );
        sei();
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
            PIN_ACCELERATOR_POSITION_SENSOR_LOW,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, HIGH );
        sei();

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
            PIN_ACCELERATOR_POSITION_SENSOR_LOW,
            PIN_ACCELERATOR_POSITION_SENSOR_HIGH );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, LOW );
        sei();

        g_throttle_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


static void read_accelerator_position_sensor(
    accelerator_position_s * const value )
{
    cli();
    value->high = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_HIGH );
    value->low = analogRead( PIN_ACCELERATOR_POSITION_SENSOR_LOW );
    sei();
}


static bool check_voltage_grounded( uint16_t high, uint16_t low ) {

    static unsigned long elapsed_detection_time = 0;
    unsigned long current_time = millis();

    bool ret = false;
    if( (high == 0) || (low == 0) )
    {
        if ( elapsed_detection_time == 0 )
        {
          elapsed_detection_time = millis();
        }

        if( (current_time - elapsed_detection_time) > FAULT_HYSTERESIS )
        {
          ret = true;
        }
    }
    else
    {
      elapsed_detection_time = 0;
    }

    return ret;
}
