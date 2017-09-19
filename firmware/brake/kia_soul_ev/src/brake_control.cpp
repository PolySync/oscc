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
#include "vehicles.h"


/*
 * @brief Number of consecutive faults that can occur when reading the
 *        sensors before control is disabled.
 *
 */
#define SENSOR_VALIDITY_CHECK_FAULT_COUNT ( 4 )


static void read_brake_pedal_position_sensor(
    brake_pedal_position_s * const value );


void check_for_operator_override( void )
{
    if ( g_brake_control_state.enabled == true
        || g_brake_control_state.operator_override == true )
    {
        brake_pedal_position_s brake_pedal_position;

        read_brake_pedal_position_sensor( &brake_pedal_position );

        uint32_t brake_pedal_position_average =
            (brake_pedal_position.low + brake_pedal_position.high) / 2;

        if ( brake_pedal_position_average >= BRAKE_PEDAL_OVERRIDE_THRESHOLD )
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
            DTC_CLEAR(
                g_brake_control_state.dtcs,
                OSCC_BRAKE_DTC_OPERATOR_OVERRIDE );

            g_brake_control_state.operator_override = false;
        }
    }
}


void check_for_sensor_faults( void )
{
    if ( (g_brake_control_state.enabled == true)
        || DTC_CHECK(g_brake_control_state.dtcs, OSCC_BRAKE_DTC_INVALID_SENSOR_VAL) )
    {
        static int fault_count = 0;

        brake_pedal_position_s brake_pedal_position;

        read_brake_pedal_position_sensor( &brake_pedal_position );

        // sensor pins tied to ground - a value of zero indicates disconnection
        if( (brake_pedal_position.high == 0)
            || (brake_pedal_position.low == 0) )
        {
            ++fault_count;

            if( fault_count >= SENSOR_VALIDITY_CHECK_FAULT_COUNT )
            {
                disable_control( );

                DTC_SET(
                    g_brake_control_state.dtcs,
                    OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );

                publish_fault_report( );

                DEBUG_PRINTLN( "Bad value read from brake pedal position sensor" );
            }
        }
        else
        {
            DTC_CLEAR(
                    g_brake_control_state.dtcs,
                    OSCC_BRAKE_DTC_INVALID_SENSOR_VAL );

            fault_count = 0;
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
            PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, HIGH );
        sei();

        g_brake_command_timeout = false;
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
            PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH,
            PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW );

        cli();
        digitalWrite( PIN_SPOOF_ENABLE, LOW );
        sei();

        g_brake_command_timeout = false;
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
