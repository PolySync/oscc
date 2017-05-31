/**
 * @file steering_can_protocol.h
 * @brief Steering CAN Protocol.
 *
 */


#ifndef _OSCC_STEERING_CAN_PROTOCOL_H_
#define _OSCC_STEERING_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief Steering command message (CAN frame) ID.
 *
 */
#define OSCC_COMMAND_STEERING_CAN_ID (0x64)

/*
 * @brief Steering report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_STEERING_CAN_ID (0x65)

/*
 * @brief Steering report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_STEERING_CAN_DLC (8)

/*
 * @brief Steering report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC (20)


/**
 * @brief Steering command message data.
 *
 * Message size (CAN frame DLC): 8 bytes
 *
 */
typedef struct
{
    int16_t commanded_steering_wheel_angle; /*!< Steering wheel angle command.
                                             * Positive means to the
                                             * left (counter clockwise).
                                             * [0.1 degrees per bit] */

    uint8_t commanded_steering_wheel_angle_rate; /*!< Steering wheel angle rate
                                                  * command.
                                                  * Value zero means no limit.
                                                  * Value 0x01 means 2 degrees/second.
                                                  * Value 0xFA means 500 degrees/second.
                                                  * [2 degrees/second per bit] */

    uint8_t enabled : 1; /*!< Steering control command/request enabled.
                          * Value zero means off/disabled.
                          * Value one means on/enabled. */

    uint8_t reserved_0 : 1; /*!< Reserved. */

    uint8_t reserved_1 : 1; /*!< Reserved. */

    uint8_t reserved_2 : 5; /*!< Reserved. */

    uint8_t reserved_3; /*!< Reserved. */

    uint8_t reserved_4; /*!< Reserved. */

    uint8_t reserved_5; /*!< Reserved. */

    uint8_t reserved_6; /*!< Reserved. */
} oscc_command_steering_data_s;


/**
 * @brief Steering command message.
 *
 * CAN frame ID: \ref OSCC_COMMAND_STEERING_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when command was received by the firmware. */

    oscc_command_steering_data_s data; /* CAN frame data. */
} oscc_command_steering_s;


/**
 * @brief Steering report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_STEERING_CAN_DLC
 *
 */
typedef struct
{

    int16_t current_steering_wheel_angle; /*!< Steering wheel angle reported by
                                           * vehicle.
                                           * Positive means to the left
                                           * (counter clockwise).
                                           * [0.1 degrees per bit] */

    int16_t commanded_steering_wheel_angle; /*!< Steering wheel angle command.
                                             * Positive means to the left
                                             * (counter clockwise).
                                             * [0.1 degrees per bit] */

    uint16_t reserved_0; /*!< Reserved. */

    int8_t spoofed_torque_output; /*!< Spoofed steering wheel torque output to the
                                   * vehicle. [0.0625 Newton meters per bit] */

    uint8_t enabled : 1; /*!< Steering controls enabled state.
                          * Value zero means off/disabled (commands are ignored).
                          * Value one means on/enabled (no timeouts or overrides have occured). */

    uint8_t override : 1; /*!< Driver override state.
                           * Value zero means controls are provided autonomously (no override).
                           * Value one means controls are provided by the driver. */

    uint8_t reserved_1 : 1; /*!< Reserved. */

    uint8_t reserved_2 : 1; /*!< Reserved. */

    uint8_t fault_obd_timeout : 1; /*!< OBD timeout indicator.
                                   * Value zero means no timeout occurred.
                                   * Value one means timeout occurred. */

    uint8_t reserved_3 : 1; /*!< Reserved */

    uint8_t reserved_4 : 1; /*!< Reserved. */

    uint8_t reserved_5 : 1; /*!< Reserved. */
} oscc_report_steering_data_s;


/**
 * @brief Steering report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_STEERING_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_steering_data_s data; /* CAN frame data. */
} oscc_report_steering_s;


#endif /* _OSCC_STEERING_CAN_PROTOCOL_H_ */
