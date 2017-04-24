#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_


#include "can.h"
#include "mcp_can.h"


#define CAN_GATEWAY_NODE_ID (0x10)
#define CAN_GATEWAY_HARDWARE_VERSION (0x1)
#define CAN_GATEWAY_FIRMWARE_VERSION (0x1)


#define SET_WARNING(data, x) (((oscc_heartbeat_data_s*) &data)->warning_register |= ((uint16_t) x))
#define CLEAR_WARNING(data, x) (((oscc_heartbeat_data_s*) &data)->warning_register &= ~((uint16_t) x))
#define SET_ERROR(data, x) (((oscc_heartbeat_data_s*) &data)->error_register |= ((uint16_t) x))
#define CLEAR_ERROR(data, x) (((oscc_heartbeat_data_s*) &data)->error_register &= ~((uint16_t) x))
#define SET_STATE(data, x) (((oscc_heartbeat_data_s*) &data)->state = ((uint8_t) x))
#define GET_STATE(data) (((oscc_heartbeat_data_s*) &data)->state)
#define SET_CHASSIS_FLAG(data, x) (((oscc_chassis_state1_data_s*) &data)->flags |= ((uint8_t) x))
#define CLEAR_CHASSIS_FLAG(data, x) (((oscc_chassis_state1_data_s*) &data)->flags &= ~((uint8_t) x))


void publish_heartbeat_frame( void );

void publish_chassis_state1_frame( void );

void publish_chassis_state2_frame( void );

void publish_timed_tx_frames( void );

void process_kia_status1(
    const can_frame_s * const rx_frame );

void process_kia_status2(
    const can_frame_s * const rx_frame );

void process_kia_status3(
    const can_frame_s * const rx_frame );

void process_kia_status4(
    const can_frame_s * const rx_frame );

void handle_ready_rx_frames(
    const can_frame_s * const rx_frame );

void check_rx_timeouts( void );

#endif
