/**
 * @file timers.cpp
 *
 */


#include <Arduino.h>

#include "globals.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "communications.h"
#include "oscc_timer.h"
#include "timers.h"


/*
 * @brief Fault checking frequency. [Hz]
 *
 */
#define FAULT_CHECK_FREQUENCY_IN_HZ ( 1 )


static void check_for_faults( void );


void start_timers( void )
{
    timer1_init( FAULT_CHECK_FREQUENCY_IN_HZ, check_for_faults );
    timer2_init( OSCC_BRAKE_REPORT_PUBLISH_FREQ_IN_HZ, publish_brake_report );
}


static void check_for_faults( void )
{
    cli();

    check_for_sensor_faults( );

    sei();
}
