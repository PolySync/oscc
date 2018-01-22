/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "oscc_timer.h"
#include "oscc_can.h"
#include "oscc_serial.h"


void init_globals( void )
{
    g_throttle_control_state.enabled = false;
    g_throttle_control_state.operator_override = false;
    g_throttle_control_state.dtcs = 0;
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
    // Set buffer 0 to filter only throttle module and global messages
    g_control_can.init_Mask( 0, 0, 0x7F0 ); // Filter for 0x0N0 to 0x0NF
    g_control_can.init_Filt( 0, 0, OSCC_THROTTLE_CAN_ID_INDEX );
    g_control_can.init_Filt( 1, 0, OSCC_FAULT_CAN_ID_INDEX );
    // Accept only CAN Disable when buffer overflow occurs in buffer 0
    g_control_can.init_Mask( 1, 0, 0x7FF ); // Filter for one CAN ID
    g_control_can.init_Filt( 2, 1, OSCC_THROTTLE_DISABLE_CAN_ID );
}

void start_timers( void )
{
    timer1_init( OSCC_REPORT_THROTTLE_PUBLISH_FREQ_IN_HZ, publish_throttle_report );
}
