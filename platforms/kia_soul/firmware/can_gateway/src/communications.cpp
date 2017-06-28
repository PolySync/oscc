/**
 * @file communications.cpp
 *
 */


#include "gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "brake_can_protocol.h"
#include "steering_can_protocol.h"
#include "throttle_can_protocol.h"
#include "mcp_can.h"
#include "oscc_can.h"
#include "oscc_time.h"

#include "globals.h"
#include "communications.h"
#include "obd_can_protocol.h"
#include "display.h"


static void publish_heartbeat_report( void );

static void publish_chassis_state_1_report( void );

static void publish_chassis_state_2_report( void );

static void process_obd_steering_wheel_angle(
    const uint8_t * const data );

static void process_obd_wheel_speed(
    const uint8_t * const data );

static void process_obd_brake_pressure(
    const uint8_t * const data );

static void process_obd_turn_signal(
    const uint8_t * const data );

static void process_brake_report(
    const uint8_t * const data );

static void process_steering_report(
    const uint8_t * const data );

static void process_throttle_report(
    const uint8_t * const data );

static void process_rx_frame(
    const can_frame_s * const rx_frame );


void publish_reports( void )
{
    uint32_t delta = 0;

    delta = get_time_delta( g_tx_heartbeat.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_heartbeat_report( );
    }

    delta = get_time_delta( g_tx_chassis_state_1.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_chassis_state_1_report( );
    }

    delta = get_time_delta( g_tx_chassis_state_2.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_chassis_state_2_report( );
    }
}


void check_for_obd_timeout( void )
{
    bool timeout = false;

    timeout = is_timeout(
            g_obd_steering_wheel_angle_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    }

    timeout = is_timeout(
            g_obd_wheel_speed_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_WHEEL_SPEED_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID );
    }

    timeout = is_timeout(
            g_obd_brake_pressure_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_BRAKE_PRESSURE_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID );
    }

    timeout = is_timeout(
            g_obd_turn_signal_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_TURN_SIGNAL_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON );
    }
}


void check_for_incoming_message( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_obd_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


void static publish_heartbeat_report( void )
{
    g_tx_heartbeat.id = (OSCC_REPORT_HEARTBEAT_CAN_ID + OSCC_MODULE_CAN_GATEWAY_NODE_ID);
    g_tx_heartbeat.dlc = OSCC_REPORT_HEARTBEAT_CAN_DLC;
    g_tx_heartbeat.data.hardware_version = OSCC_MODULE_CAN_GATEWAY_VERSION_HARDWARE;
    g_tx_heartbeat.data.firmware_version = OSCC_MODULE_CAN_GATEWAY_VERSION_FIRMWARE;

    g_control_can.sendMsgBuf(
        g_tx_heartbeat.id,
        CAN_STANDARD,
        g_tx_heartbeat.dlc,
        (uint8_t *) &g_tx_heartbeat.data );

    g_tx_heartbeat.timestamp = GET_TIMESTAMP_MS();
}


static void publish_chassis_state_1_report( void )
{
    g_tx_chassis_state_1.id = OSCC_REPORT_CHASSIS_STATE_1_CAN_ID;
    g_tx_chassis_state_1.dlc = OSCC_REPORT_CHASSIS_STATE_1_CAN_DLC;

    g_control_can.sendMsgBuf(
            g_tx_chassis_state_1.id,
            CAN_STANDARD,
            g_tx_chassis_state_1.dlc,
            (uint8_t *) &g_tx_chassis_state_1.data );

    g_tx_chassis_state_1.timestamp = GET_TIMESTAMP_MS();
}


static void publish_chassis_state_2_report( void )
{
    g_tx_chassis_state_2.id = OSCC_REPORT_CHASSIS_STATE_2_CAN_ID;
    g_tx_chassis_state_2.dlc = OSCC_REPORT_CHASSIS_STATE_2_CAN_DLC;

    g_control_can.sendMsgBuf(
            g_tx_chassis_state_2.id,
            CAN_STANDARD,
            g_tx_chassis_state_2.dlc,
            (uint8_t *) &g_tx_chassis_state_2.data );

    g_tx_chassis_state_2.timestamp = GET_TIMESTAMP_MS();
}


static void process_obd_steering_wheel_angle(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        kia_soul_obd_steering_wheel_angle_data_s * steering_wheel_angle_data =
            (kia_soul_obd_steering_wheel_angle_data_s *) data;

        CLEAR_HEARTBEAT_WARNING( KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT );
        SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID );

        g_tx_chassis_state_1.data.steering_wheel_angle_rate = 0;
        g_tx_chassis_state_1.data.steering_wheel_angle = steering_wheel_angle_data->steering_angle;

        g_obd_steering_wheel_angle_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_obd_wheel_speed(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        kia_soul_obd_wheel_speed_data_s * wheel_speed_data =
            (kia_soul_obd_wheel_speed_data_s *) data;

        CLEAR_HEARTBEAT_WARNING( KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT );
        SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID );

        g_tx_chassis_state_2.data.wheel_speed_front_left = wheel_speed_data->wheel_speed_front_left;
        g_tx_chassis_state_2.data.wheel_speed_front_right = wheel_speed_data->wheel_speed_front_right;
        g_tx_chassis_state_2.data.wheel_speed_rear_left = wheel_speed_data->wheel_speed_rear_left;
        g_tx_chassis_state_2.data.wheel_speed_rear_right = wheel_speed_data->wheel_speed_rear_right;

        g_obd_wheel_speed_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_obd_brake_pressure(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        kia_soul_obd_brake_pressure_data_s * brake_pressure_data =
            (kia_soul_obd_brake_pressure_data_s *) data;

        CLEAR_HEARTBEAT_WARNING( KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT );
        SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID );

        g_tx_chassis_state_1.data.brake_pressure = brake_pressure_data->master_cylinder_pressure;

        g_obd_brake_pressure_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_obd_turn_signal(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        kia_soul_obd_turn_signal_data_s * turn_signal_data =
            (kia_soul_obd_turn_signal_data_s *) data;

        CLEAR_HEARTBEAT_WARNING( KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON );

        if( turn_signal_data->turn_signal_flags == KIA_SOUL_OBD_TURN_SIGNAL_FLAG_LEFT_TURN )
        {
            SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON );
        }
        else if( turn_signal_data->turn_signal_flags == KIA_SOUL_OBD_TURN_SIGNAL_FLAG_RIGHT_TURN )
        {
            SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON );
        }

        g_obd_turn_signal_rx_timestamp = GET_TIMESTAMP_MS( );
    }
}


static void process_brake_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        oscc_report_brake_data_s * brake_report_data =
            (oscc_report_brake_data_s *) data;

        if ( brake_report_data->enabled == true )
        {
            g_display_state.status_screen.brake_status = MODULE_STATUS_ENABLED;
        }
        else
        {
            g_display_state.status_screen.brake_status = MODULE_STATUS_DISABLED;
        }
    }
}


static void process_steering_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        oscc_report_steering_data_s * steering_report_data =
            (oscc_report_steering_data_s *) data;

        if ( steering_report_data->enabled == true )
        {
            g_display_state.status_screen.steering_status = MODULE_STATUS_ENABLED;
        }
        else
        {
            g_display_state.status_screen.steering_status = MODULE_STATUS_DISABLED;
        }
    }
}



static void process_throttle_report(
    const uint8_t * const data )
{
    if ( data != NULL )
    {
        oscc_report_throttle_data_s * throttle_report_data =
            (oscc_report_throttle_data_s *) data;

        if ( throttle_report_data->enabled == true )
        {
            g_display_state.status_screen.throttle_status = MODULE_STATUS_ENABLED;
        }
        else
        {
            g_display_state.status_screen.throttle_status = MODULE_STATUS_DISABLED;
        }
    }
}



static void process_rx_frame(
    const can_frame_s * const rx_frame )
{
    if ( rx_frame != NULL )
    {
        if( rx_frame->id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID )
        {
            process_obd_steering_wheel_angle( rx_frame->data );
        }
        else if( rx_frame->id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID )
        {
            process_obd_wheel_speed( rx_frame->data );
        }
        else if( rx_frame->id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID )
        {
            process_obd_brake_pressure( rx_frame->data );
        }
        else if( rx_frame->id == KIA_SOUL_OBD_TURN_SIGNAL_CAN_ID )
        {
            process_obd_turn_signal( rx_frame->data );
        }
        else if( rx_frame->id == OSCC_REPORT_BRAKE_CAN_ID )
        {
            process_brake_report( rx_frame->data );
        }
        else if( rx_frame->id == OSCC_REPORT_STEERING_CAN_ID )
        {
            process_steering_report( rx_frame->data );
        }
        else if( rx_frame->id == OSCC_REPORT_THROTTLE_CAN_ID )
        {
            process_throttle_report( rx_frame->data );
        }
    }
}
