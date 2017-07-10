/**
 * @file obd_can_protocol.h
 * @brief Kia Soul OBD-II CAN Protocol.
 *
 */


#ifndef _KIA_SOUL_PLATFORM_INFO_H_
#define _KIA_SOUL_PLATFORM_INFO_H_


#include <stdint.h>


/*
 * @brief ID of the Kia Soul's OBD steering wheel angle CAN frame.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID (0x2B0)


/*
 * @brief ID of the Kia Soul's OBD wheel speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID (0x4B0)


/*
 * @brief ID of the Kia Soul's OBD brake pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID (0x220)


/**
 * @brief Steering wheel angle message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t steering_wheel_angle; /* 1/10th of a degree per bit. */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_steering_wheel_angle_data_s;


/**
 * @brief Wheel speed message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t wheel_speed_front_left; /* 1/128 mph per bit */

    int16_t wheel_speed_front_right; /* 1/128 mph per bit */

    int16_t wheel_speed_rear_left; /* 1/128 mph per bit */

    int16_t wheel_speed_rear_right; /* 1/128 mph per bit */
} kia_soul_obd_wheel_speed_data_s;


/**
 * @brief Brake pressure message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t master_cylinder_pressure; /* 1/10th of a bar per bit */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_brake_pressure_data_s;


#endif
