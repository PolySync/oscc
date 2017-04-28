/**
 * @file gateway_can_protocol.h
 * @brief Gateway CAN Protocol.
 *
 */


#ifndef _OSCC_GATEWAY_CAN_PROTOCOL_H_
#define _OSCC_GATEWAY_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief Node ID of CAN Gateway module.
 *
 */
#define OSCC_MODULE_CAN_GATEWAY_NODE_ID (0x10)

/*
 * @brief Hardware version of CAN Gateway module.
 *
 */
#define OSCC_MODULE_CAN_GATEWAY_VERSION_HARDWARE (0x01)

/*
 * @brief Firmware version of CAN Gateway module.
 *
 */
#define OSCC_MODULE_CAN_GATEWAY_VERSION_FIRMWARE (0x01)

/*
 * @brief Heartbeat report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_HEARTBEAT_CAN_ID (0x100)

/*
 * @brief Heartbeat report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_HEARTBEAT_CAN_DLC (8)

/*
 * @brief Heartbeat report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC (50)

/*
 * @brief Heartbeat state indicating heartbeat is invalid.
 *
 */
#define OSCC_REPORT_HEARTBEAT_STATE_INVALID (0x00)

/*
 * @brief Heartbeat state indicating heartbeat is initializing.
 *
 */
#define OSCC_REPORT_HEARTBEAT_STATE_INIT (0x01)

/*
 * @brief Heartbeat state indicating heartbeat is okay.
 *
 */
#define OSCC_REPORT_HEARTBEAT_STATE_OK (0x02)


/**
 * @brief Heartbeat report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_HEARTBEAT_CAN_DLC
 *
 */
typedef struct
{
    uint8_t hardware_version : 4; /* Module hardware version. */

    uint8_t firmware_version : 4; /* Module firmware version. */

    uint8_t state; /* Heartbeat state. */

    uint16_t reserved; /* Reserved. */

    uint16_t error_register; /* Register containing error flags. */

    uint16_t warning_register; /* Register containing warning flags. */
} oscc_report_heartbeat_data_s;


/**
 * @brief Heartbeat report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_HEARTBEAT_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_heartbeat_data_s data; /* CAN frame data. */
} oscc_report_heartbeat_s;


#endif /* _OSCC_GATEWAY_CAN_PROTOCOL_H_ */
