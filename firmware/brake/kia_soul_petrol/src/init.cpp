/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "accumulator.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/global_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "init.h"
#include "master_cylinder.h"
#include "oscc_can.h"
#include "oscc_eeprom.h"
#include "oscc_timer.h"
#include "vehicles.h"


void init_globals( void )
{
    g_brake_control_state.enabled = false;
    g_brake_control_state.operator_override = false;
    g_brake_control_state.dtcs = 0;

    g_brake_command_timeout = false;

    pid_zeroize( &g_pid, BRAKE_PID_WINDUP_GUARD );
    g_pid.proportional_gain = BRAKE_PID_PROPORTIONAL_GAIN;
    g_pid.integral_gain = BRAKE_PID_INTEGRAL_GAIN;
    g_pid.derivative_gain = BRAKE_PID_DERIVATIVE_GAIN;
}


void init_devices( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    #ifndef TESTS
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4
    #endif

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
    g_control_can.init_Mask( 0, 0, 0x7F0 ); // Filter for 0x0N0 to 0x0NF
    g_control_can.init_Mask( 1, 0, 0x7FF ); // Don't use second filter
    g_control_can.init_Filt( 0, 0, OSCC_BRAKE_CAN_ID_INDEX );
    g_control_can.init_Filt( 1, 0, OSCC_GLOBAL_CAN_ID_INDEX );
}


void init_config( void )
{
    #ifdef RESET_CONFIG
    DEBUG_PRINT( "Resetting config to defaults");

    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MIN, BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN);
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MAX, BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MAX );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_PETROL_FAULT_CHECK_FREQUENCY_IN_HZ, BRAKE_FAULT_CHECK_FREQUENCY_IN_HZ );
    oscc_eeprom_write_u16( OSCC_CONFIG_U16_BRAKE_PETROL_REPORT_PUBLISH_FREQUENCY_IN_HZ, OSCC_BRAKE_REPORT_PUBLISH_FREQUENCY_IN_HZ );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS, BRAKE_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS, BRAKE_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS, BRAKE_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS, BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MIN_IN_DECIBARS, BRAKE_PRESSURE_MIN_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MAX_IN_DECIBARS, BRAKE_PRESSURE_MAX_IN_DECIBARS );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_PROPORTIONAL_GAIN, BRAKE_PID_PROPORTIONAL_GAIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_INTEGRAL_GAIN, BRAKE_PID_INTEGRAL_GAIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_DERIVATIVE_GAIN, BRAKE_PID_DERIVATIVE_GAIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_WINDUP_GUARD, BRAKE_PID_WINDUP_GUARD );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MIN, BRAKE_PID_OUTPUT_MIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MAX, BRAKE_PID_OUTPUT_MAX );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN, BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX, BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MIN, BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MAX, BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MAX );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN, BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX, BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MIN, BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MIN );
    oscc_eeprom_write_f32( OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MAX, BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MAX );
    #endif

    g_eeprom_config.brake_pressure_sensor_check_value_min = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MIN );
    g_eeprom_config.brake_pressure_sensor_check_value_max = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MAX );
    g_eeprom_config.fault_check_frequency_in_hz = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_PETROL_FAULT_CHECK_FREQUENCY_IN_HZ );
    g_eeprom_config.report_publish_frequency_in_hz = oscc_eeprom_read_u16( OSCC_CONFIG_U16_BRAKE_PETROL_REPORT_PUBLISH_FREQUENCY_IN_HZ );
    g_eeprom_config.accumulator_pressure_min_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS );
    g_eeprom_config.accumulator_pressure_max_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS );
    g_eeprom_config.override_pedal_threshold_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS );
    g_eeprom_config.brake_light_pressure_threshold_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS );
    g_eeprom_config.brake_pressure_min_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MIN_IN_DECIBARS );
    g_eeprom_config.brake_pressure_max_in_decibars = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MAX_IN_DECIBARS );
    g_eeprom_config.pid_proportional_gain = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_PROPORTIONAL_GAIN );
    g_eeprom_config.pid_integral_gain = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_INTEGRAL_GAIN );
    g_eeprom_config.pid_derivative_gain = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_DERIVATIVE_GAIN );
    g_eeprom_config.pid_windup_guard = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_WINDUP_GUARD );
    g_eeprom_config.pid_output_min = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MIN );
    g_eeprom_config.pid_output_max = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MAX );
    g_eeprom_config.pid_accumulator_solenoid_clamped_min = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN );
    g_eeprom_config.pid_accumulator_solenoid_clamped_max = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX );
    g_eeprom_config.pid_release_solenoid_clamped_min = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MIN );
    g_eeprom_config.pid_release_solenoid_clamped_max = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MAX );
    g_eeprom_config.accumulator_solenoid_duty_cycle_min = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN );
    g_eeprom_config.accumulator_solenoid_duty_cycle_max = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX );
    g_eeprom_config.release_solenoid_duty_cycle_min = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MIN );
    g_eeprom_config.release_solenoid_duty_cycle_max = oscc_eeprom_read_f32( OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MAX );
}
