#include "gateway_protocol_can.h"
#include "mcp_can.h"
#include "can.h"
#include "time.h"

#include "communications.h"
#include "obd_can_protocol.h"


void publish_heartbeat_frame( can_frame_s *frame, MCP_CAN &control_can )
{
    oscc_heartbeat_data_s * const data =
            (oscc_heartbeat_data_s*) frame->data;

    frame->id = (uint32_t) (OSCC_CAN_ID_HEARTBEAT + CAN_GATEWAY_NODE_ID);
    frame->dlc = OSCC_CAN_DLC_HEARTBEAT;
    data->hardware_version = CAN_GATEWAY_HARDWARE_VERSION;
    data->firmware_version = CAN_GATEWAY_FIRMWARE_VERSION;

    control_can.sendMsgBuf(
            frame->id,
            0, // standard ID (not extended)
            frame->dlc,
            frame->data );

    frame->timestamp = GET_TIMESTAMP_MS();
}


void publish_chassis_state1_frame( can_frame_s *frame, MCP_CAN &control_can )
{
    frame->id = (uint32_t) OSCC_CAN_ID_CHASSIS_STATE_1;
    frame->dlc = OSCC_CAN_DLC_CHASSIS_STATE_1;

    control_can.sendMsgBuf(
            frame->id,
            0, // standard ID (not extended)
            frame->dlc,
            frame->data );

    frame->timestamp = GET_TIMESTAMP_MS();
}


void publish_chassis_state2_frame( can_frame_s *frame, MCP_CAN &control_can )
{
    frame->id = (uint32_t) OSCC_CAN_ID_CHASSIS_STATE_2;
    frame->dlc = OSCC_CAN_DLC_CHASSIS_STATE_2;

    control_can.sendMsgBuf(
            frame->id,
            0, // standard ID (not extended)
            frame->dlc,
            frame->data );

    frame->timestamp = GET_TIMESTAMP_MS();
}


void publish_timed_tx_frames(
    can_frame_s *tx_frame_heartbeat,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    MCP_CAN &control_can )
{
    uint32_t delta = 0;

    delta = get_time_delta( tx_frame_heartbeat->timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_PUBLISH_INTERVAL_HEARTBEAT )
    {
        publish_heartbeat_frame( tx_frame_heartbeat, control_can);
    }

    delta = get_time_delta( tx_frame_chassis_state1->timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_PUBLISH_INTERVAL_CHASSIS_STATE_1 )
    {
        publish_chassis_state1_frame( tx_frame_chassis_state1, control_can );
    }

    delta = get_time_delta( tx_frame_chassis_state2->timestamp, GET_TIMESTAMP_MS() );
    if( delta >= OSCC_PUBLISH_INTERVAL_CHASSIS_STATE_2 )
    {
        publish_chassis_state2_frame( tx_frame_chassis_state2, control_can );
    }
}


void process_kia_status1(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame )
{
    int32_t s32 = 0;

    // copy to global frame
    memcpy(
            &rx_frame_kia_status1,
            rx_frame,
            sizeof(*rx_frame) );

    const kia_obd_status1_data_s * const kia_data =
            (kia_obd_status1_data_s*) rx_frame_kia_status1->data;

    oscc_chassis_state1_data_s * const control_data =
            (oscc_chassis_state1_data_s*) tx_frame_chassis_state1->data;

    CLEAR_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT );

    SET_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID );
    //SET_CHASSIS_FLAG( PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );

    CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    control_data->steering_wheel_angle_rate = 0;

    s32 = (int32_t) kia_data->steering_angle;
    control_data->steering_wheel_angle = (int16_t) s32;

#warning "TODO - steering wheel angle convertion and rate calculation"
}


void process_kia_status2(
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame )
{
    int32_t s32 = 0;

    // copy to global frame
    memcpy(
            &rx_frame_kia_status2,
            rx_frame,
            sizeof(*rx_frame) );

    const kia_obd_status2_data_s * const kia_data =
            (kia_obd_status2_data_s*) rx_frame_kia_status2->data;

    oscc_chassis_state2_data_s * const control_data =
            (oscc_chassis_state2_data_s*) tx_frame_chassis_state2->data;

    CLEAR_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT );

    SET_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID );

#warning "TODO - wheel speed unit convertion"
    // convert left front wheel speed
    s32 = (int32_t) kia_data->wheel_speed_lf;
    control_data->wheel_speed_lf = (int16_t) s32;

    // convert right front wheel speed
    s32 = (int32_t) kia_data->wheel_speed_rf;
    control_data->wheel_speed_rf = (int16_t) s32;

    // convert left rear wheel speed
    s32 = (int32_t) kia_data->wheel_speed_lr;
    control_data->wheel_speed_lr = (int16_t) s32;

    // convert right rear wheel speed
    s32 = (int32_t) kia_data->wheel_speed_rr;
    control_data->wheel_speed_rr = (int16_t) s32;
}


void process_kia_status3(
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame )
{
    int16_t s16 = 0;

    // copy to global frame
    memcpy(
            &rx_frame_kia_status3,
            rx_frame,
            sizeof(*rx_frame) );

    const kia_obd_status3_data_s * const kia_data =
            (kia_obd_status3_data_s*) rx_frame_kia_status3->data;

    oscc_chassis_state1_data_s * const control_data =
            (oscc_chassis_state1_data_s*) tx_frame_chassis_state1->data;

    CLEAR_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT );

    SET_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID );

#warning "TODO - brake pressure unit convertion"
    // convert
    s16 = (int16_t) kia_data->master_cylinder_pressure;
    control_data->brake_pressure = (int16_t) s16;
}


void process_kia_status4(
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame )
{
    // copy to global frame
    memcpy(
            &rx_frame_kia_status4,
            rx_frame,
            sizeof(*rx_frame) );

    const kia_obd_status4_data_s * const kia_data =
            (kia_obd_status4_data_s*) rx_frame_kia_status4->data;

    oscc_chassis_state1_data_s * const control_data =
            (oscc_chassis_state1_data_s*) tx_frame_chassis_state1->data;

    CLEAR_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT );

    CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
    CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
    CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON );

    if( kia_data->turn_signal_flags == KIA_CCAN_STATUS4_TURN_SIGNAL_LEFT )
    {
        SET_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
    }

    if( kia_data->turn_signal_flags == KIA_CCAN_STATUS4_TURN_SIGNAL_RIGHT )
    {
        SET_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
    }
}


void handle_ready_rx_frames(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    can_frame_s *tx_frame_heartbeat,
    can_frame_s *frame )
{
    // check for a supported frame ID
    if( frame->id == KIA_CCAN_STATUS1_MESSAGE_ID )
    {
        // process status1
        process_kia_status1(
            rx_frame_kia_status1,
            tx_frame_chassis_state1,
            tx_frame_heartbeat,
            frame );
    }
    else if( frame->id == KIA_CCAN_STATUS2_MESSAGE_ID )
    {
        // process status2
        process_kia_status2(
            rx_frame_kia_status2,
            tx_frame_chassis_state1,
            tx_frame_chassis_state2,
            tx_frame_heartbeat,
            frame );
    }
    else if( frame->id == KIA_CCAN_STATUS3_MESSAGE_ID )
    {
        // process status3
        process_kia_status3(
            rx_frame_kia_status3,
            tx_frame_chassis_state1,
            tx_frame_heartbeat,
            frame );
    }
    else if( frame->id == KIA_CCAN_STATUS4_MESSAGE_ID )
    {
        // process status4
        process_kia_status4(
            rx_frame_kia_status4,
            tx_frame_chassis_state1,
            tx_frame_heartbeat,
            frame );
    }
}


void check_rx_timeouts(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat )
{
    bool timeout = false;

    timeout = is_timeout(
            rx_frame_kia_status1->timestamp,
            GET_TIMESTAMP_MS(),
            KIA_CCAN_STATUS1_RX_WARN_TIMEOUT);

    if( timeout == true )
    {
        SET_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID );
    }

    timeout = is_timeout(
            rx_frame_kia_status2->timestamp,
            GET_TIMESTAMP_MS(),
            KIA_CCAN_STATUS2_RX_WARN_TIMEOUT);

    if( timeout == true )
    {
        SET_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID );
    }

    timeout = is_timeout(
            rx_frame_kia_status3->timestamp,
            GET_TIMESTAMP_MS(),
            KIA_CCAN_STATUS3_RX_WARN_TIMEOUT);

    if( timeout == true )
    {
        SET_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID );
    }

    timeout = is_timeout(
            rx_frame_kia_status4->timestamp,
            GET_TIMESTAMP_MS(),
            KIA_CCAN_STATUS4_RX_WARN_TIMEOUT);

    if( timeout == true )
    {
        SET_WARNING( tx_frame_heartbeat->data, OSCC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON );
        CLEAR_CHASSIS_FLAG( tx_frame_chassis_state1->data, PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON );
    }
}
