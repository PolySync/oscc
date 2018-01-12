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
    timer1_init( g_eeprom_config.fault_check_frequency_in_hz, check_for_faults );

    timer2_init( g_eeprom_config.report_publish_frequency_in_hz, publish_steering_report );
}


static void check_for_faults( void )
{
    cli();

    check_for_controller_command_timeout( );

    check_for_sensor_faults( );

    g_steering_command_timeout = true;

    sei();
}
