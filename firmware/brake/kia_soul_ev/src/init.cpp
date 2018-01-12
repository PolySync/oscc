/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/global_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "oscc_can.h"
#include "oscc_eeprom.h"
#include "vehicles.h"


void init_globals( void )
{
    g_brake_control_state.enabled = false;
    g_brake_control_state.operator_override = false;
    g_brake_control_state.dtcs = 0;

    g_brake_command_timeout = false;
}


void init_devices( void )
{
    pinMode( PIN_DAC_CHIP_SELECT, OUTPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SENSOR_HIGH, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SENSOR_LOW, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SPOOF_HIGH, INPUT );
    pinMode( PIN_BRAKE_PEDAL_POSITION_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );
    pinMode( PIN_BRAKE_LIGHT_ENABLE, OUTPUT );

    cli();
    digitalWrite( PIN_DAC_CHIP_SELECT, HIGH );
    digitalWrite( PIN_SPOOF_ENABLE, LOW );
    digitalWrite( PIN_BRAKE_LIGHT_ENABLE, LOW );
    sei();
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( g_control_can );

    // Filter CAN messages - accept if (CAN_ID & mask) == (filter & mask)
    g_control_can.init_Mask( 0, 0, 0x7F0 ); // Filter for 0x0N0 to 0x0NF
    g_control_can.init_Mask( 1, 0, 0x7FF ); // Don't use second filter
    g_control_can.init_Filt( 0, 0, OSCC_BRAKE_CAN_ID_INDEX );
    g_control_can.init_Filt( 1, 0, OSCC_GLOBAL_CAN_ID_INDEX );
}


void init_config( void )
{
    #ifdef RESET_CONFIG
    DEBUG_PRINT( "Resetting config to defaults");

    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_LOW_SIGNAL_RANGE_MIN, BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_LOW_SIGNAL_RANGE_MAX, BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_HIGH_SIGNAL_RANGE_MIN, BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_SPOOF_HIGH_SIGNAL_RANGE_MAX, BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_PEDAL_OVERRIDE_THRESHOLD, BRAKE_PEDAL_OVERRIDE_THRESHOLD );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_LOW_THRESHOLD, BRAKE_LIGHT_SPOOF_LOW_THRESHOLD );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_HIGH_THRESHOLD, BRAKE_LIGHT_SPOOF_HIGH_THRESHOLD );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_FAULT_CHECK_FREQUENCY_IN_HZ, BRAKE_FAULT_CHECK_FREQUENCY_IN_HZ );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_EV_REPORT_PUBLISH_FREQ_IN_HZ, OSCC_BRAKE_REPORT_PUBLISH_FREQ_IN_HZ );
    #endif

    g_eeprom_config.spoof_low_signal_range_min = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN );
    g_eeprom_config.spoof_low_signal_range_max = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX );
    g_eeprom_config.spoof_high_signal_range_min = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN );
    g_eeprom_config.spoof_high_signal_range_max = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX );
    g_eeprom_config.pedal_override_threshold = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_PEDAL_OVERRIDE_THRESHOLD );
    g_eeprom_config.brake_light_spoof_low_threshold = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_LOW_THRESHOLD );
    g_eeprom_config.brake_light_spoof_high_threshold = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_HIGH_THRESHOLD );
    g_eeprom_config.fault_check_frequency_in_hz = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_FAULT_CHECK_FREQUENCY_IN_HZ );
    g_eeprom_config.report_publish_frequency_in_hz = oscc_eeprom_read_u16( OSCC_CONFIG_U16_THROTTLE_REPORT_PUBLISH_FREQUENCY_IN_HZ );
}
