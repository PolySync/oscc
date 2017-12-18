/**
 * @file brake_control.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/global_can_protocol.h"
#include "brake_control.h"
#include "communications.h"
#include "debug.h"
#include "dtc.h"
#include "globals.h"
#include "oscc_dac.h"
#include "oscc_eeprom.h"
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

        uint16_t brake_pedal_override_threshold =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_PEDAL_OVERRIDE_THRESHOLD );

        if ( brake_pedal_position_average >= brake_pedal_override_threshold )
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
        uint16_t brake_spoof_high_signal_range_min =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_HIGH_SIGNAL_RANGE_MIN );

        uint16_t brake_spoof_high_signal_range_max =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_HIGH_SIGNAL_RANGE_MAX );

        uint16_t spoof_high =
            constrain(
                spoof_command_high,
                brake_spoof_high_signal_range_min,
                brake_spoof_high_signal_range_max );


        uint16_t brake_spoof_low_signal_range_min =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_LOW_SIGNAL_RANGE_MIN );

        uint16_t brake_spoof_low_signal_range_max =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_LOW_SIGNAL_RANGE_MAX );

        uint16_t spoof_low =
            constrain(
                spoof_command_low,
                brake_spoof_low_signal_range_min,
                brake_spoof_low_signal_range_max );


        uint16_t brake_light_spoof_low_threshold =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_LOW_THRESHOLD );

        uint16_t brake_light_spoof_high_threshold =
            oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_HIGH_THRESHOLD );

        if( (spoof_high > brake_light_spoof_high_threshold)
            || (spoof_low > brake_light_spoof_low_threshold) )
        {
            cli();
            digitalWrite( PIN_BRAKE_LIGHT_ENABLE, HIGH );
            sei();
        }
        else
        {
            cli();
            digitalWrite( PIN_BRAKE_LIGHT_ENABLE, LOW );
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
