/**
 * @file obd_can_protocol.h
 * @brief Kia Soul OBD-II CAN Protocol.
 *
 */


#ifndef _KIA_SOUL_OBD_CAN_PROTOCOL_H_
#define _KIA_SOUL_OBD_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief ID of the Kia Soul's OBD steering wheel angle CAN frame.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID (0x2B0)

/*
 * @brief Amount of time between steering wheel angle CAN frames considered to
 *        be a timeout.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_RX_WARN_TIMEOUT_IN_MSEC (50)

/*
 * @brief Bit in heartbeat warning register corresponding to steering wheel
 *        angle.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_HEARTBEAT_WARNING_BIT (0x0100)

/*
 * @brief ID of the Kia Soul's OBD wheel speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID (0x4B0)

/*
 * @brief Amount of time between wheel speed CAN frames considered to
 *        be a timeout.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_RX_WARN_TIMEOUT_IN_MSEC (50)

/*
 * @brief Bit in heartbeat warning register corresponding to wheel speed.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_HEARTBEAT_WARNING_BIT (0x0200)

/*
 * @brief ID of the Kia Soul's OBD brake pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID (0x220)

/*
 * @brief Amount of time between brake pressure CAN frames considered to
 *        be a timeout.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_RX_WARN_TIMEOUT_IN_MSEC (50)

/*
 * @brief Bit in heartbeat warning register corresponding to brake pressure.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_WARNING_BIT (0x0400)

/*
 * @brief ID of the Kia Soul's OBD turn signal CAN frame.
 *
 */
#define KIA_SOUL_OBD_TURN_SIGNAL_CAN_ID (0x18)

/*
 * @brief Amount of time between turn signal CAN frames considered to
 *        be a timeout.
 *
 */
#define KIA_SOUL_OBD_TURN_SIGNAL_RX_WARN_TIMEOUT_IN_MSEC (500)

/*
 * @brief Bit in heartbeat warning register corresponding to turn signal.
 *
 */
#define KIA_SOUL_OBD_TURN_SIGNAL_WARNING_BIT (0x0800)

/*
 * @brief Turn signal flag representing a left turn.
 *
 */
#define KIA_SOUL_OBD_TURN_SIGNAL_FLAG_LEFT_TURN (0x0C)

/*
 * @brief Turn signal flag representing a right turn.
 *
 */
#define KIA_SOUL_OBD_TURN_SIGNAL_FLAG_RIGHT_TURN (0x0A)


/**
 * @brief Steering wheel angle message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t steering_angle; /* 1/10th of a degree per bit. */

    uint16_t reserved_0; /* Reserved. */

    uint16_t reserved_1; /* Reserved. */

    uint16_t reserved_2; /* Reserved. */
} kia_soul_obd_steering_wheel_angle_data_s;


/**
 * @brief Steering wheel angle message.
 *
 * CAN frame ID: \ref KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when report was received by the firmware. */

    kia_soul_obd_steering_wheel_angle_data_s data; /* CAN frame data. */
} kia_soul_obd_steering_wheel_angle_s;


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
 * @brief Wheel speed message.
 *
 * CAN frame ID: \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when report was received by the firmware. */

    kia_soul_obd_wheel_speed_data_s data; /* CAN frame data. */
} kia_soul_obd_wheel_speed_s;


/**
 * @brief Brake pressure message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    uint16_t reserved_0; /* Reserved. */

    uint16_t reserved_1; /* Reserved. */

    int16_t master_cylinder_pressure; /* 1/10th of a bar per bit */

    uint16_t reserved_2; /* Reserved. */
} kia_soul_obd_brake_pressure_data_s;


/**
 * @brief Brake pressure message.
 *
 * CAN frame ID: \ref KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when report was received by the firmware. */

    kia_soul_obd_brake_pressure_data_s data; /* CAN frame data. */
} kia_soul_obd_brake_pressure_s;


/**
 * @brief Turn signal message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    uint16_t reserved_0; /* Reserved. */

    uint16_t reserved_1; /* Reserved. */

    uint8_t reserved_2; /* Reserved. */

    uint8_t reserved_3 : 4; /* Reserved. */

    uint8_t turn_signal_flags : 4; /* Turn signal flags. */

    uint16_t reserved_4; /* Reserved. */
} kia_soul_obd_turn_signal_data_s;


/**
 * @brief Turn signal message.
 *
 * CAN frame ID: \ref KIA_SOUL_OBD_TURN_SIGNAL_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when report was received by the firmware. */

    kia_soul_obd_turn_signal_data_s data; /* CAN frame data. */
} kia_soul_obd_turn_signal_s;


#endif
