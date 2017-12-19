/**
 * @file communications.cpp
 *
 */


#include <Arduino.h>
#include <stdint.h>

#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/global_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "mcp_can.h"
#include "oscc_can.h"
#include "oscc_eeprom.h"


static void process_rx_frame(
    const can_frame_s * const frame );

static void process_brake_command(
    const uint8_t * const data );

static void process_fault_report(
    const uint8_t * const data );

static void process_config_u16(
    const uint8_t * const data );

static void process_config_f32(
    const uint8_t * const data );


void publish_brake_report( void )
{
    oscc_brake_report_s brake_report;

    brake_report.magic[0] = (uint8_t) OSCC_MAGIC_BYTE_0;
    brake_report.magic[1] = (uint8_t) OSCC_MAGIC_BYTE_1;
    brake_report.enabled = (uint8_t) g_brake_control_state.enabled;
    brake_report.operator_override = (uint8_t) g_brake_control_state.operator_override;
    brake_report.dtcs = g_brake_control_state.dtcs;

    cli();
    g_control_can.sendMsgBuf(
        OSCC_BRAKE_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_BRAKE_REPORT_CAN_DLC,
        (uint8_t *) &brake_report );
    sei();
}


void publish_fault_report( void )
{
    oscc_fault_report_s fault_report;

    fault_report.magic[0] = (uint8_t) OSCC_MAGIC_BYTE_0;
    fault_report.magic[1] = (uint8_t) OSCC_MAGIC_BYTE_1;
    fault_report.fault_origin_id = FAULT_ORIGIN_BRAKE;
    fault_report.dtcs = g_brake_control_state.dtcs;

    cli();
    g_control_can.sendMsgBuf(
        OSCC_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
    sei();
}


void check_for_controller_command_timeout( void )
{
    if( g_brake_control_state.enabled == true )
    {
        if ( g_brake_command_timeout == true )
        {
            disable_control( );

            publish_fault_report( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


void check_for_incoming_message( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


static void process_rx_frame(
    const can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if( (frame->data[0] == OSCC_MAGIC_BYTE_0)
             && (frame->data[1] == OSCC_MAGIC_BYTE_1) )
        {
            if ( frame->id == OSCC_BRAKE_ENABLE_CAN_ID )
            {
                enable_control( );
            }
            else if ( frame->id == OSCC_BRAKE_DISABLE_CAN_ID )
            {
                disable_control( );
            }
            else if ( frame->id == OSCC_BRAKE_COMMAND_CAN_ID )
            {
                process_brake_command( frame->data );
            }
            else if ( frame->id == OSCC_FAULT_REPORT_CAN_ID )
            {
                process_fault_report( frame->data );
            }
            else if ( frame->id == OSCC_CONFIG_U16_CAN_ID )
            {
                process_config_u16( frame->data );
            }
            else if ( frame->id == OSCC_CONFIG_F32_CAN_ID )
            {
                process_config_f32( frame->data );
            }
        }
    }
}


static void process_brake_command(
    const uint8_t * const data )
{
    if (data != NULL )
    {
        const oscc_brake_command_s * const brake_command =
                (oscc_brake_command_s *) data;

        g_brake_control_state.commanded_pedal_position =
            brake_command->pedal_command;

        g_brake_command_timeout = false;
    }
}


static void process_fault_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_fault_report_s * const fault_report =
                (oscc_fault_report_s *) data;

        disable_control( );

        DEBUG_PRINT( "Fault report received from: " );
        DEBUG_PRINT( fault_report->fault_origin_id );
        DEBUG_PRINT( "  DTCs: ");
        DEBUG_PRINTLN( fault_report->dtcs );
    }
}


static void process_config_u16(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_config_u16_s * const config =
                (oscc_config_u16_s *) data;

        if ( (config->name == OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MIN)
            || (config->name == OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MAX)
            || (config->name == OSCC_CONFIG_U16_BRAKE_PETROL_FAULT_CHECK_FREQUENCY_IN_HZ)
            || (config->name == OSCC_CONFIG_U16_BRAKE_PETROL_REPORT_PUBLISH_FREQ_IN_HZ) )
        {
            oscc_eeprom_write_u16( config->name, config->value );
        }
    }
}


static void process_config_f32(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_config_f32_s * const config =
                (oscc_config_f32_s *) data;

        if ( (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MIN_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MAX_IN_DECIBARS)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_PROPORTIONAL_GAIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_INTEGRAL_GAIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_DERIVATIVE_GAIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_WINDUP_GUARD)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MAX)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MAX)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MIN)
            || (config->name == OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MAX) )
        {
            oscc_eeprom_write_f32( config->name, config->value );
        }
    }
}
