#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_


#include "can.h"
#include "mcp_can.h"
#include "gateway_protocol_can.h"


#define SET_WARNING(data, x) (((oscc_report_heartbeat_data_s*) &data)->warning_register |= ((uint16_t) x))
#define CLEAR_WARNING(data, x) (((oscc_report_heartbeat_data_s*) &data)->warning_register &= ~((uint16_t) x))
#define SET_ERROR(data, x) (((oscc_report_heartbeat_data_s*) &data)->error_register |= ((uint16_t) x))
#define CLEAR_ERROR(data, x) (((oscc_report_heartbeat_data_s*) &data)->error_register &= ~((uint16_t) x))
#define SET_STATE(data, x) (((oscc_report_heartbeat_data_s*) &data)->state = ((uint8_t) x))
#define GET_STATE(data) (((oscc_report_heartbeat_data_s*) &data)->state)
#define SET_CHASSIS_FLAG(data, x) (((oscc_report_chassis_state_1_data_s*) &data)->flags |= ((uint8_t) x))
#define CLEAR_CHASSIS_FLAG(data, x) (((oscc_report_chassis_state_1_data_s*) &data)->flags &= ~((uint8_t) x))


void publish_heartbeat_frame( void );

void publish_chassis_state_1_frame( void );

void publish_chassis_state_2_frame( void );

void publish_timed_tx_frames( void );

void process_obd_steering_wheel_angle(
    const uint8_t * const data );

void process_obd_wheel_speed(
    const uint8_t * const data );

void process_obd_brake_pressure(
    const uint8_t * const data );

void process_obd_turn_signal(
    const uint8_t * const data );

void handle_ready_rx_frames(
    const can_frame_s * const rx_frame );

void check_rx_timeouts( void );

#endif
