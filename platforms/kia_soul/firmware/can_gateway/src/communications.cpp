#include "gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "mcp_can.h"
#include "can.h"
#include "time.h"

#include "globals.h"
#include "communications.h"
#include "obd_can_protocol.h"


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

static void process_rx_frame(
    const can_frame_s * const rx_frame );


void publish_heartbeat_report( void )
{
    tx_heartbeat.data.hardware_version = OSCC_MODULE_CAN_GATEWAY_VERSION_HARDWARE;
    tx_heartbeat.data.firmware_version = OSCC_MODULE_CAN_GATEWAY_VERSION_FIRMWARE;

    control_can.sendMsgBuf(
            tx_heartbeat.id,
            CAN_STANDARD,
            tx_heartbeat.dlc,
            (uint8_t *) &tx_heartbeat.data );

    tx_heartbeat.timestamp = GET_TIMESTAMP_MS();
}


void publish_reports( void )
{
    uint32_t delta = 0;

    delta = get_time_delta( tx_heartbeat.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_heartbeat_report( );
    }

    delta = get_time_delta( tx_chassis_state_1.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_chassis_state_1_report( );
    }

    delta = get_time_delta( tx_chassis_state_2.timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC )
    {
        publish_chassis_state_2_report( );
    }
}


void check_for_controller_command_timeout( void )
{
    bool timeout = false;

    timeout = is_timeout(
            obd_steering_wheel_angle_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    }

    timeout = is_timeout(
            obd_wheel_speed_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_WHEEL_SPEED_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID );
    }

    timeout = is_timeout(
            obd_brake_pressure_rx_timestamp,
            GET_TIMESTAMP_MS(),
            KIA_SOUL_OBD_BRAKE_PRESSURE_RX_WARN_TIMEOUT_IN_MSEC);

    if( timeout == true )
    {
        SET_HEARTBEAT_WARNING( KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT );
        CLEAR_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID );
    }

    timeout = is_timeout(
            obd_turn_signal_rx_timestamp,
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
    can_status_t ret = check_for_rx_frame( obd_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        process_rx_frame( &rx_frame );
    }
}


static void publish_chassis_state_1_report( void )
{
    control_can.sendMsgBuf(
            tx_chassis_state_1.id,
            CAN_STANDARD,
            tx_chassis_state_1.dlc,
            (uint8_t *) &tx_chassis_state_1.data );

    tx_chassis_state_1.timestamp = GET_TIMESTAMP_MS();
}


static void publish_chassis_state_2_report( void)
{
    control_can.sendMsgBuf(
            tx_chassis_state_2.id,
            CAN_STANDARD,
            tx_chassis_state_2.dlc,
            (uint8_t *) &tx_chassis_state_2.data );

    tx_chassis_state_2.timestamp = GET_TIMESTAMP_MS();
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

        #warning "TODO - steering wheel angle conversion and rate calculation"

        tx_chassis_state_1.data.steering_wheel_angle_rate = 0;
        tx_chassis_state_1.data.steering_wheel_angle = steering_wheel_angle_data->steering_angle;

        obd_steering_wheel_angle_rx_timestamp = GET_TIMESTAMP_MS( );
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

        #warning "TODO - wheel speed unit conversion"

        tx_chassis_state_2.data.wheel_speed_lf = wheel_speed_data->wheel_speed_lf;
        tx_chassis_state_2.data.wheel_speed_rf = wheel_speed_data->wheel_speed_rf;
        tx_chassis_state_2.data.wheel_speed_lr = wheel_speed_data->wheel_speed_lr;
        tx_chassis_state_2.data.wheel_speed_rr = wheel_speed_data->wheel_speed_rr;

        obd_wheel_speed_rx_timestamp = GET_TIMESTAMP_MS( );
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

        #warning "TODO - brake pressure unit conversion"

        tx_chassis_state_1.data.brake_pressure = brake_pressure_data->master_cylinder_pressure;

        obd_brake_pressure_rx_timestamp = GET_TIMESTAMP_MS( );
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

        if( turn_signal_data->turn_signal_flags == KIA_SOUL_OBD_TURN_SIGNAL_FLAG_RIGHT_TURN )
        {
            SET_CHASSIS_1_FLAG( OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON );
        }

        obd_turn_signal_rx_timestamp = GET_TIMESTAMP_MS( );
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
    }
}
