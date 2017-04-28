/**
 * @file gateway_protocol_can.h
 * @brief Gateway CAN Protocol.
 *
 */


#ifndef _OSCC_GATEWAY_PROTOCOL_CAN_H_
#define _OSCC_GATEWAY_PROTOCOL_CAN_H_


#include <stdint.h>


#define OSCC_MODULE_CAN_GATEWAY_NODE_ID (0x10)
#define OSCC_MODULE_CAN_GATEWAY_VERSION_HARDWARE (0x01)
#define OSCC_MODULE_CAN_GATEWAY_VERSION_FIRMWARE (0x01)

#define OSCC_REPORT_HEARTBEAT_CAN_ID (0x100)
#define OSCC_REPORT_HEARTBEAT_CAN_DLC (8)
#define OSCC_REPORT_HEARTBEAT_PUBLISH_INTERVAL_IN_MSEC (50)
#define OSCC_REPORT_HEARTBEAT_STATE_INVALID (0x00)
#define OSCC_REPORT_HEARTBEAT_STATE_INIT (0x01)
#define OSCC_REPORT_HEARTBEAT_STATE_OK (0x02)


typedef struct
{
    uint8_t hardware_version : 4;
    uint8_t firmware_version : 4;
    uint8_t state;
    uint16_t reserved;
    uint16_t error_register;
    uint16_t warning_register;
} oscc_report_heartbeat_data_s;


typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t timestamp;
    oscc_report_heartbeat_data_s data;
} oscc_report_heartbeat_s;


#endif /* _OSCC_GATEWAY_PROTOCOL_CAN_H_ */
