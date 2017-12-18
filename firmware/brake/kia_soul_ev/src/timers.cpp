/**
 * @file timers.cpp
 *
 */


#include <Arduino.h>

#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/global_can_protocol.h"
#include "communications.h"
#include "globals.h"
#include "oscc_timer.h"
#include "oscc_eeprom.h"
#include "timers.h"


static void check_for_faults( void );


void start_timers( void )
{
    uint16_t fault_check_frequency_in_hz =
        oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_FAULT_CHECK_FREQUENCY_IN_HZ );

    timer1_init( fault_check_frequency_in_hz, check_for_faults );


    uint16_t report_publish_frequency_in_hz =
        oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_EV_REPORT_PUBLISH_FREQ_IN_HZ );

    timer2_init( report_publish_frequency_in_hz, publish_brake_report );
}


static void check_for_faults( void )
{
    cli();

    check_for_controller_command_timeout( );

    check_for_sensor_faults( );

    g_brake_command_timeout = true;

    sei();
}
