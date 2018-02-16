/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>

#include "can_protocols/brake_can_protocol.h"
#include "brake_control.h"
#include "communications.h"
#include "debug.h"
#include "dtc.h"
#include "globals.h"
#include "oscc_dac.h"
#include "oscc_check.h"
#include "vehicles.h"




static void read_brake_pedal_position_sensor(
    brake_pedal_position_s * const value );


void check_for_faults( void )
{
    static condition_state_s grounded_fault_state = CONDITION_STATE_INIT;
    static condition_state_s operator_override_state = CONDITION_STATE_INIT;

    brake_pedal_position_s brake_pedal_position;

    if ( (g_brake_control_state.enabled == true)
        || (g_brake_control_state.dtcs > 0) )
    {
        read_brake_pedal_position_sensor( &brake_pedal_position );

        uint32_t brake_pedal_position_average =
            (brake_pedal_position.low + brake_pedal_position.high) / 2;

        bool operator_overridden = condition_exceeded_duration(
                brake_pedal_position_average >= BRAKE_PEDAL_OVERRIDE_THRESHOLD,
                FAULT_HYSTERESIS,
                &operator_override_state);

        bool inputs_grounded = check_voltage_grounded(
                brake_pedal_position.high,
                brake_pedal_position.low,
                FAULT_HYSTERESIS,
                &grounded_fault_state);
        // sensor pins tied to ground - a value of zero indicates disconnection
        if( inputs_grounded == true )
        {
            disable_control( );

            DTC_SET(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );

            publish_fault_report( );

            DEBUG_PRINTLN( "Bad value read from brake pedal position sensor" );
        }
        else if ( operator_overridden == true )
        {
            disable_control( );

            DTC_SET(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_OPERATOR_OVERRIDE );

            publish_fault_report( );

            g_brake_control_state.operator_override = true;

            DEBUG_PRINTLN( "Operator override" );
        }
        else
        {
            g_brake_control_state.dtcs = 0;

            g_brake_control_state.operator_override = false;
        }
    }
}


void update_brake(
    uint16_t spoof_command_high,
    uint16_t spoof_command_low )
{
    if ( g_brake_control_state.enabled == true )
    {
        uint16_t spoof_high =
            constrain(
                spoof_command_high,
                BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN,
                BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX );

        uint16_t spoof_low =
            constrain(
                spoof_command_low,
                BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN,
                BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX );

        if( (spoof_high > BRAKE_LIGHT_SPOOF_HIGH_THRESHOLD)
            || (spoof_low > BRAKE_LIGHT_SPOOF_LOW_THRESHOLD) )
        {
            cli();
            digitalWrite(PIN_BRAKE_LIGHT_ENABLE, HIGH);
            sei();
        }
        else
        {
            cli();
            digitalWrite(PIN_BRAKE_LIGHT_ENABLE, LOW);
            sei();
        }

        cli();
        g_dac.outputA( spoof_high );
        g_dac.outputB( spoof_low );
        sei();
    }
}


void enable_control( void )
{
    if( g_brake_control_state.enabled == false
        && g_brake_control_state.operator_override == false )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, HIGH );
        sei();

        g_brake_control_state.enabled = true;

        DEBUG_PRINTLN( "Control enabled" );
    }
}


void disable_control( void )
{
    if( g_brake_control_state.enabled == true )
    {
        const uint16_t num_samples = 20;
        prevent_signal_discontinuity(
            g_dac,
            num_samples,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, LOW );
        digitalWrite( PIN_BRAKE_LIGHT_ENABLE, LOW );
        sei();

        g_brake_control_state.enabled = false;

        DEBUG_PRINTLN( "Control disabled" );
    }
}


static void read_brake_pedal_position_sensor(
    brake_pedal_position_s * const value )
{
    cli();
    value->high = analogRead( PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH );
    value->low = analogRead( PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW );
    sei();
}
