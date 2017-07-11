/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "fault_can_protocol.h"
#include "steering_can_protocol.h"
#include "oscc_time.h"
#include "debug.h"
#include "kia_soul.h"

#include "globals.h"
#include "communications.h"
#include "steering_control.h"


static void process_steering_command(
    const uint8_t * const data );

static void process_obd_steering_wheel_angle(
    const uint8_t * const data );

static void process_rx_frame(
    can_frame_s * const frame );

static void check_for_controller_command_timeout( void );

static void check_for_obd_timeout( void );


void publish_steering_report( void )
{
    uint32_t delta = get_time_delta( g_steering_report_last_tx_timestamp, GET_TIMESTAMP_MS() );

    if ( delta >= OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC )
    {
        oscc_report_steering_s steering_report;

        steering_report.id = OSCC_REPORT_STEERING_CAN_ID;
        steering_report.dlc = OSCC_REPORT_STEERING_CAN_DLC;
        steering_report.data.enabled = (uint8_t) g_steering_control_state.enabled;
        steering_report.data.override = (uint8_t) g_steering_control_state.operator_override;
        steering_report.data.current_steering_wheel_angle = (int16_t) g_steering_control_state.current_steering_wheel_angle;
        steering_report.data.commanded_steering_wheel_angle = (int16_t) g_steering_control_state.commanded_steering_wheel_angle;
        steering_report.data.fault_obd_timeout = (uint8_t) g_steering_control_state.obd_timeout;
        steering_report.data.spoofed_torque_output = (int8_t) g_spoofed_torque_output_sum;
        steering_report.data.fault_invalid_sensor_value = (uint8_t) g_steering_control_state.invalid_sensor_value;

        g_control_can.sendMsgBuf(
            steering_report.id,
            CAN_STANDARD,
            steering_report.dlc,
            (uint8_t *) &steering_report.data );

        g_steering_report_last_tx_timestamp = GET_TIMESTAMP_MS( );
    }
}


void publish_fault_report( void )
{
    oscc_module_fault_report_s fault_report;

    fault_report.fault_origin_id = FAULT_ORIGIN_STEERING;

    g_control_can.sendMsgBuf(
        OSCC_MODULE_FAULT_REPORT_CAN_ID,
        CAN_STANDARD,
        OSCC_MODULE_FAULT_REPORT_CAN_DLC,
        (uint8_t *) &fault_report );
}


void check_for_can_frame( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_control_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


void check_for_timeouts( void )
{
    check_for_controller_command_timeout( );

    check_for_obd_timeout( );
}


static void process_steering_command(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const oscc_command_steering_data_s * const steering_command_data =
                (oscc_command_steering_data_s *) data;

        if ( steering_command_data->enabled == true )
        {
            // divisor values found empirically to best match steering output
            g_steering_control_state.commanded_steering_wheel_angle =
                (steering_command_data->commanded_steering_wheel_angle / 9.0);

            g_steering_control_state.commanded_steering_wheel_angle_rate =
                (steering_command_data->commanded_steering_wheel_angle_rate * 9.0);

            DEBUG_PRINT( "controller commanded steering wheel angle: " );
            DEBUG_PRINTLN( g_steering_control_state.commanded_steering_wheel_angle );

            enable_control( );
        }
        else
        {
            disable_control( );
        }

        g_steering_command_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_obd_steering_wheel_angle(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        const kia_soul_obd_steering_wheel_angle_data_s * const steering_wheel_angle_data =
                (kia_soul_obd_steering_wheel_angle_data_s *) data;

        g_steering_control_state.current_steering_wheel_angle =
            steering_wheel_angle_data->steering_wheel_angle
            * RAW_ANGLE_SCALAR
            * WHEEL_ANGLE_TO_STEERING_WHEEL_ANGLE_SCALAR;

        g_obd_steering_wheel_angle_last_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_rx_frame(
    can_frame_s * const frame )
{
    if ( frame != NULL )
    {
        if ( frame->id == OSCC_COMMAND_STEERING_CAN_ID )
        {
            process_steering_command( frame->data );
        }
        else if ( frame->id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID )
        {
            process_obd_steering_wheel_angle( frame->data );
        }
        else if ( frame->id == OSCC_MODULE_FAULT_REPORT_CAN_ID )
        {
            disable_control( );

            DEBUG_PRINTLN( "Fault report received" );
        }
    }
}


static void check_for_controller_command_timeout( void )
{
    if( g_steering_control_state.enabled == true )
    {
        bool timeout = is_timeout(
                g_steering_command_last_rx_timestamp,
                GET_TIMESTAMP_MS( ),
                COMMAND_TIMEOUT_IN_MSEC);

        if( timeout == true )
        {
            disable_control( );

            publish_fault_report( );

            DEBUG_PRINTLN( "Timeout - controller command" );
        }
    }
}


static void check_for_obd_timeout( void )
{
    bool timeout = is_timeout(
            g_obd_steering_wheel_angle_last_rx_timestamp,
            GET_TIMESTAMP_MS( ),
            OBD_TIMEOUT_IN_MSEC );

    if( timeout == true )
    {
        disable_control( );

        publish_fault_report( );

        g_steering_control_state.obd_timeout = true;

        DEBUG_PRINTLN( "Timeout - OBD - steering wheel angle" );
    }
    else
    {
        g_steering_control_state.obd_timeout = false;
    }
}
