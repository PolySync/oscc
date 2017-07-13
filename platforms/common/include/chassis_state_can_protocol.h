/**
 * @file chassis_state_can_protocol.h
 * @brief Chassis State CAN Protocol.
 *
 */


#ifndef _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_
#define _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief Chassis State 1 report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_CAN_ID (0x210)

/*
 * @brief Chassis State 1 report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_CAN_DLC (8)

/*
 * @brief Chassis State 1 report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_PUBLISH_INTERVAL_IN_MSEC (50)

/*
 * @brief Chassis State 1 report message flag indicating valid steering wheel angle.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_VALID (0x02)

/*
 * @brief Chassis State 1 report message flag indicating valid steering wheel angle rate.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_STEER_WHEEL_ANGLE_RATE_VALID (0x04)

/*
 * @brief Chassis State 1 report message flag indicating valid brake pressure.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_PRESSURE_VALID (0x08)

/*
 * @brief Chassis State 1 report message flag indicating valid wheel speed.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_WHEEL_SPEED_VALID (0x10)

/*
 * @brief Chassis State 1 report message flag indicating left turn signal is on.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_LEFT_TURN_SIGNAL_ON (0x20)

/*
 * @brief Chassis State 1 report message flag indicating right turn signal is on.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_RIGHT_TURN_SIGNAL_ON (0x40)

/*
 * @brief Chassis State 1 report message flag indicating brake signal is on.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_1_FLAGS_BIT_BRAKE_SIGNAL_ON (0x80)

/*
 * @brief Chassis State 2 report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_2_CAN_ID (0x211)

/*
 * @brief Chassis State 2 report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_2_CAN_DLC (8)

/*
 * @brief Chassis State 2 report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_CHASSIS_STATE_2_PUBLISH_INTERVAL_IN_MSEC (50)


/**
 * @brief Chassis State 1 report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_CHASSIS_STATE_1_CAN_DLC
 *
 */
typedef struct
{
    uint8_t flags; /* Stauts flags. */

    uint8_t reserved; /* Reserved. */

    int16_t steering_wheel_angle; /* Steering wheel angle. */

    int16_t steering_wheel_angle_rate; /* Steering wheel angle rate. */

    int16_t brake_pressure; /* Brake pressure. */
} oscc_report_chassis_state_1_data_s;


/**
 * @brief Chassis State 1 report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_CHASSIS_STATE_1_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_chassis_state_1_data_s data; /* CAN frame data. */
} oscc_report_chassis_state_1_s;


/**
 * @brief Chassis State 2 report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_CHASSIS_STATE_2_CAN_DLC
 *
 */
typedef struct
{
    int16_t wheel_speed_front_left; /* Speed of front left wheel. */

    int16_t wheel_speed_front_right; /* Speed of front right wheel. */

    int16_t wheel_speed_rear_left; /* Speed of rear left wheel. */

    int16_t wheel_speed_rear_right; /* Speed of rear right wheel. */
} oscc_report_chassis_state_2_data_s;


/**
 * @brief Chassis State 2 report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_CHASSIS_STATE_2_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_chassis_state_2_data_s data; /* CAN frame data. */
} oscc_report_chassis_state_2_s;


#endif /* _OSCC_CHASSIS_STATE_CAN_PROTOCOL_H_ */
