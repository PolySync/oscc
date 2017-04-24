/**
 * @file gateway_protocol_can.h
 * @brief Gateway CAN Protocol.
 *
 */


#ifndef GATEWAY_PROTOCOL_CAN_H
#define GATEWAY_PROTOCOL_CAN_H


#include <stdint.h>


#define OSCC_CAN_ID_HEARTBEAT (0x100)
#define OSCC_CAN_DLC_HEARTBEAT (8)
#define OSCC_PUBLISH_INTERVAL_HEARTBEAT (50)
#define OSCC_CAN_ID_CHASSIS_STATE_1 (0x210)
#define OSCC_CAN_DLC_CHASSIS_STATE_1 (8)
#define OSCC_PUBLISH_INTERVAL_CHASSIS_STATE_1 (50) // ms
#define OSCC_CAN_ID_CHASSIS_STATE_2 (0x211)
#define OSCC_CAN_DLC_CHASSIS_STATE_2 (8)
#define OSCC_PUBLISH_INTERVAL_CHASSIS_STATE_2 (50) // ms
#define OSCC_HEARTBEAT_STATE_INVALID (0x00)
#define OSCC_HEARTBEAT_STATE_INIT (0x01)
#define OSCC_HEARTBEAT_STATE_OK (0x02)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID (0x02)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID (0x04)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID (0x08)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID (0x10)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON (0x20)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON (0x40)
#define PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON (0x80)
#define KIA_STATUS1_MESSAGE_ID (0x210)


typedef struct
{
    uint8_t hardware_version : 4;
    uint8_t firmware_version : 4;
    uint8_t state;
    uint16_t reserved;
    uint16_t error_register;
    uint16_t warning_register;
} oscc_heartbeat_data_s;


typedef struct
{
    uint8_t flags;
    uint8_t reserved_0;
    int16_t steering_wheel_angle;
    int16_t steering_wheel_angle_rate;
    int16_t brake_pressure;
} oscc_chassis_state1_data_s;


typedef struct
{
    int16_t wheel_speed_lf;
    int16_t wheel_speed_rf;
    int16_t wheel_speed_lr;
    int16_t wheel_speed_rr;
} oscc_chassis_state2_data_s;


#endif
