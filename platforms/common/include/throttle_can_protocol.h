/**
 * @file throttle_can_protocol.h
 * @brief Throttle CAN Protocol.
 *
 */


#ifndef _OSCC_THROTTLE_CAN_PROTOCOL_H_
#define _OSCC_THROTTLE_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief Throttle command message (CAN frame) ID.
 *
 */
#define OSCC_COMMAND_THROTTLE_CAN_ID (0x62)


/*
 * @brief Throttle report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_THROTTLE_CAN_ID (0x63)


/*
 * @brief Throttle report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_THROTTLE_CAN_DLC (8)


/*
 * @brief Throttle report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_THROTTLE_PUBLISH_INTERVAL_IN_MSEC (20)


/**
 * @brief Throttle command message data.
 *
 * Message size (CAN frame DLC): 8 bytes
 *
 */
typedef struct
{
    uint16_t commanded_accelerator_position; /*!< Accelerator position command.
                                            * [65535 == 100%] */

    uint8_t reserved_0; /*!< Reserved. */

    uint8_t enabled : 1; /*!< Throttle control command/request enabled.
                          * Value zero means off/disabled.
                          * Value one means on/enabled. */

    uint8_t reserved_1 : 1; /*!< Reserved. */

    uint8_t reserved_2 : 1; /*!< Reserved. */

    uint8_t reserved_3 : 5; /*!< Reserved. */

    uint8_t reserved_4; /*!< Reserved. */

    uint8_t reserved_5; /*!< Reserved. */

    uint8_t reserved_6; /*!< Reserved. */

    uint8_t reserved_7; /*!< Reserved. */
} oscc_command_throttle_data_s;


/**
 * @brief Throttle command message.
 *
 * CAN frame ID: \ref OSCC_COMMAND_THROTTLE_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when command was received by the firmware. */

    oscc_command_throttle_data_s data; /* CAN frame data. */
} oscc_command_throttle_s;


/**
 * @brief Throttle report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_THROTTLE_CAN_DLC
 *
 */
typedef struct
{
    uint16_t current_accelerator_position; /*!< Current accelerator position as
                                            * read by the acceleration position
                                            * sensor. [65535 == 100%] */

    uint16_t commanded_accelerator_position; /*!< Commanded accelerator position
                                              * from the throttle command message.
                                              * [65535 == 100%] */

    uint16_t spoofed_accelerator_output; /*!< Spoof accelerator value output to
                                          * the vehicle.
                                          * [65535 == 100%] */

    uint8_t reserved_0 : 4; /*!< Reserved. */

    uint8_t reserved_1 : 4; /*!< Reserved. */

    uint8_t enabled : 1; /*!< Throttle controls enabled state.
                          * Value zero means off/disabled (commands are ignored).
                          * Value one means on/enabled (no timeouts or overrides have occured). */

    uint8_t override : 1; /*!< Driver override state.
                           * Value zero means controls are provided autonomously (no override).
                           * Value one means controls are provided by the driver. */

    uint8_t reserved_2 : 1; /*!< Reserved. */

    uint8_t reserved_3 : 1; /*!< Reserved. */

    uint8_t reserved_4 : 1; /*!< Reserved. */

    uint8_t reserved_5 : 1; /*!< Reserved. */

    uint8_t reserved_6 : 1; /*!< Reserved. */

    uint8_t reserved_7 : 1; /*!< Reserved. */
} oscc_report_throttle_data_s;


/**
 * @brief Throttle report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_THROTTLE_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_throttle_data_s data; /* CAN frame data. */
} oscc_report_throttle_s;


#endif /* _OSCC_THROTTLE_CAN_PROTOCOL_H_ */
