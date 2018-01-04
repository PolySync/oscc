/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/global_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "vehicles.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "oscc_can.h"
#include "oscc_eeprom.h"


void init_globals( void )
{
    g_throttle_control_state.enabled = false;
    g_throttle_control_state.operator_override = false;
    g_throttle_control_state.dtcs = 0;

    g_throttle_command_timeout = false;
}


void init_devices( void )
{
    pinMode( PIN_DAC_CHIP_SELECT, OUTPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SENSOR_HIGH, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SENSOR_LOW, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SPOOF_HIGH, INPUT );
    pinMode( PIN_ACCELERATOR_POSITION_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    cli();
    digitalWrite( PIN_DAC_CHIP_SELECT, HIGH );
    digitalWrite( PIN_SPOOF_ENABLE, LOW );
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
    g_control_can.init_Filt( 0, 0, OSCC_THROTTLE_CAN_ID_INDEX );
    g_control_can.init_Filt( 1, 0, OSCC_GLOBAL_CAN_ID_INDEX );
}


void init_config( void )
{
    DEBUG_PRINT( "Resetting config to defaults");

    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN, THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX, THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN, THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX, THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_PEDAL_OVERRIDE_THRESHOLD, THROTTLE_PEDAL_OVERRIDE_THRESHOLD );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_FAULT_CHECK_FREQUENCY_IN_HZ, THROTTLE_FAULT_CHECK_FREQUENCY_IN_HZ );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_THROTTLE_REPORT_PUBLISH_FREQ_IN_HZ, OSCC_THROTTLE_REPORT_PUBLISH_FREQ_IN_HZ );

}
