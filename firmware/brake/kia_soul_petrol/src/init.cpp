/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "accumulator.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "master_cylinder.h"
#include "oscc_can.h"
#include "oscc_serial.h"
#include "oscc_timer.h"
#include "vehicles.h"


void init_globals( void )
{
    g_brake_control_state.enabled = false;
    g_brake_control_state.operator_override = false;
    g_brake_control_state.dtcs = 0;

    pid_zeroize( &g_pid, BRAKE_PID_WINDUP_GUARD );
    g_pid.proportional_gain = BRAKE_PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain = BRAKE_PID_INTEGRAL_GAIN;
    g_pid.derivative_gain = BRAKE_PID_DERIVATIVE_GAIN;
}


void init_devices( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    accumulator_init( );
    master_cylinder_init( );
    brake_init( );

    accumulator_turn_pump_off( );
    master_cylinder_open( );

    set_release_solenoid_duty_cycle( SOLENOID_PWM_OFF );
    set_accumulator_solenoid_duty_cycle( SOLENOID_PWM_OFF );
}


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    DEBUG_PRINT( "init Control CAN - " );
    init_can( g_control_can );

    // Filter CAN messages - accept if (CAN_ID & mask) == (filter & mask)
    // Set buffer 0 to filter only brake module and global messages
    g_control_can.init_Mask( 0, 0, 0x7F0 ); // Filter for 0x0N0 to 0x0NF
    g_control_can.init_Filt( 0, 0, OSCC_BRAKE_CAN_ID_INDEX );
    g_control_can.init_Filt( 1, 0, OSCC_FAULT_CAN_ID_INDEX );
    // Accept only CAN Disable when buffer overflow occurs in buffer 0
    g_control_can.init_Mask( 1, 0, 0x7FF ); // Filter for one CAN ID
    g_control_can.init_Filt( 2, 1, OSCC_BRAKE_DISABLE_CAN_ID );
}
