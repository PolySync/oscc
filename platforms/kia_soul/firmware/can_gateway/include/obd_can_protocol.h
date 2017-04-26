/**
 * @file obd_can_protocol.h
 * @brief Kia Soul OBDII CAN Protocol.
 *
 */


#ifndef _KIA_SOUL_OBD_CAN_PROTOCOL_H_
#define _KIA_SOUL_OBD_CAN_PROTOCOL_H_


#include <stdint.h>


#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID (0x2B0)
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_RX_WARN_TIMEOUT_IN_MSEC (50)
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT (0x0100)

#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID (0x4B0)
#define KIA_SOUL_OBD_WHEEL_SPEED_RX_WARN_TIMEOUT_IN_MSEC (50)
#define KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT (0x0200)

#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID (0x220)
#define KIA_SOUL_OBD_BRAKE_PRESSURE_RX_WARN_TIMEOUT_IN_MSEC (50)
#define KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT (0x0400)

#define KIA_SOUL_OBD_TURN_SIGNAL_CAN_ID (0x18)
#define KIA_SOUL_OBD_TURN_SIGNAL_RX_WARN_TIMEOUT_IN_MSEC (500)
#define KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT (0x0800)
#define KIA_SOUL_OBD_TURN_SIGNAL_FLAG_LEFT_TURN (0x0C)
#define KIA_SOUL_OBD_TURN_SIGNAL_FLAG_RIGHT_TURN (0x0A)


// 0x2B0
// signals: ...
typedef struct
{
    int16_t steering_angle; // 1/10th of a degree per bit
    uint16_t reserved_0;
    uint16_t reserved_1;
    uint16_t reserved_2;
} kia_soul_obd_steering_wheel_angle_data_s;


typedef struct
{
    uint32_t timestamp;
    kia_soul_obd_steering_wheel_angle_data_s data;
} kia_soul_obd_steering_wheel_angle_s;


// 0x4B0
// signals: ...
typedef struct
{
    int16_t wheel_speed_lf; // 1/128 mph per bit
    int16_t wheel_speed_rf;
    int16_t wheel_speed_lr;
    int16_t wheel_speed_rr;
} kia_soul_obd_wheel_speed_data_s;


typedef struct
{
    uint32_t timestamp;
    kia_soul_obd_wheel_speed_data_s data;
} kia_soul_obd_wheel_speed_s;


// 0x220
// signals: ...
typedef struct
{
    uint16_t reserved_0;
    uint16_t reserved_1;
    int16_t master_cylinder_pressure; // 1/10th of a bar per bit
    uint16_t reserved_2;
} kia_soul_obd_brake_pressure_data_s;


typedef struct
{
    uint32_t timestamp;
    kia_soul_obd_brake_pressure_data_s data;
} kia_soul_obd_brake_pressure_s;


// 0x18
// signals: ...
typedef struct
{
    uint16_t reserved_0;
    uint16_t reserved_1;
    uint8_t reserved_2;
    uint8_t reserved_3 : 4;
    uint8_t turn_signal_flags : 4;
    uint16_t reserved_4;
} kia_soul_obd_turn_signal_data_s;


typedef struct
{
    uint32_t timestamp;
    kia_soul_obd_turn_signal_data_s data;
} kia_soul_obd_turn_signal_s;


#endif
