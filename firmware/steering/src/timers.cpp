/**
 * @file timers.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/global_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "vehicles.h"
#include "communications.h"
#include "globals.h"
#include "oscc_eeprom.h"
#include "oscc_timer.h"
#include "steering_control.h"
#include "timers.h"


static void check_for_faults( void );


void start_timers( void )
{
    uint16_t fault_check_frequency_in_hz =
        oscc_eeprom_read_u16( OSCC_CONFIG_U16_STEERING_FAULT_CHECK_FREQUENCY_IN_HZ );

    timer1_init( fault_check_frequency_in_hz, check_for_faults );


    uint16_t report_publish_frequency_in_hz =
        oscc_eeprom_read_u16( OSCC_CONFIG_U16_STEERING_REPORT_PUBLISH_FREQ_IN_HZ );

    timer2_init( report_publish_frequency_in_hz, publish_steering_report );
}


static void check_for_faults( void )
{
    cli();

    check_for_controller_command_timeout( );

    check_for_sensor_faults( );

    g_steering_command_timeout = true;

    sei();
}
