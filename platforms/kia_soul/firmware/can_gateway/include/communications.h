#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_

#include "can.h"
#include "mcp_can.h"

#define CAN_GATEWAY_NODE_ID (0x10)
#define CAN_GATEWAY_HARDWARE_VERSION (0x1)
#define CAN_GATEWAY_FIRMWARE_VERSION (0x1)

//
#define SET_WARNING(data, x) (((oscc_heartbeat_data_s*) &data)->warning_register |= ((uint16_t) x))


//
#define CLEAR_WARNING(data, x) (((oscc_heartbeat_data_s*) &data)->warning_register &= ~((uint16_t) x))


//
#define SET_ERROR(data, x) (((oscc_heartbeat_data_s*) &data)->error_register |= ((uint16_t) x))


//
#define CLEAR_ERROR(data, x) (((oscc_heartbeat_data_s*) &data)->error_register &= ~((uint16_t) x))


//
#define SET_STATE(data, x) (((oscc_heartbeat_data_s*) &data)->state = ((uint8_t) x))


//
#define GET_STATE(data) (((oscc_heartbeat_data_s*) &data)->state)

//
#define SET_CHASSIS_FLAG(data, x) (((psvc_chassis_state1_data_s*) &data)->flags |= ((uint8_t) x))


//
#define CLEAR_CHASSIS_FLAG(data, x) (((psvc_chassis_state1_data_s*) &data)->flags &= ~((uint8_t) x))

void publish_heartbeat_frame(
    can_frame_s *frame,
    MCP_CAN &control_can );

void publish_chassis_state1_frame(
    can_frame_s *frame,
    MCP_CAN &control_can );

void publish_chassis_state2_frame(
    can_frame_s *frame,
    MCP_CAN &control_can );

void publish_timed_tx_frames(
    can_frame_s *tx_frame_heartbeat,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    MCP_CAN &control_can );

void process_kia_status1(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame );

void process_kia_status2(
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame );

void process_kia_status3(
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame );

void process_kia_status4(
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat,
    const can_frame_s * const rx_frame );

void handle_ready_rx_frames(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_chassis_state2,
    can_frame_s *tx_frame_heartbeat,
    can_frame_s *frame );

void check_rx_timeouts(
    can_frame_s *rx_frame_kia_status1,
    can_frame_s *rx_frame_kia_status2,
    can_frame_s *rx_frame_kia_status3,
    can_frame_s *rx_frame_kia_status4,
    can_frame_s *tx_frame_chassis_state1,
    can_frame_s *tx_frame_heartbeat );

#endif
