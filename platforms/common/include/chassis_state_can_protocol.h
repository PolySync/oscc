/**
 * @file chassis_state_can_protocol.h
 * @brief Chassis State CAN Protocol.
 *
 */


#ifndef _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_
#define _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_


#include <stdint.h>


#define OSCC_REPORT_CHASSIS_STATE_1_CAN_ID (0x210)
#define OSCC_REPORT_CHASSIS_STATE_1_CAN_DLC (8)
#define OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC (50)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID (0x02)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID (0x04)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID (0x08)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID (0x10)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON (0x20)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON (0x40)
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON (0x80)

#define OSCC_REPORT_CHASSIS_STATE_2_CAN_ID (0x211)
#define OSCC_REPORT_CHASSIS_STATE_2_CAN_DLC (8)
#define OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC (50)


typedef struct
{
    uint8_t flags;
    uint8_t reserved_0;
    int16_t steering_wheel_angle;
    int16_t steering_wheel_angle_rate;
    int16_t brake_pressure;
} oscc_report_chassis_state_1_data_s;


typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t timestamp;
    oscc_report_chassis_state_1_data_s data;
} oscc_report_chassis_state_1_s;


typedef struct
{
    int16_t wheel_speed_lf;
    int16_t wheel_speed_rf;
    int16_t wheel_speed_lr;
    int16_t wheel_speed_rr;
} oscc_report_chassis_state_2_data_s;


typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t timestamp;
    oscc_report_chassis_state_2_data_s data;
} oscc_report_chassis_state_2_s;


#endif /* _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_ */
