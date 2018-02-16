/**
 * @file throttle_can_protocol.h
 * @brief Throttle CAN Protocol.
 *
 */


#ifndef _OSCC_THROTTLE_CAN_PROTOCOL_H_
#define _OSCC_THROTTLE_CAN_PROTOCOL_H_


#include <stdint.h>
#include "magic.h"


/*
 * @brief CAN ID representing the range of throttle messages.
 *
 */
#define OSCC_THROTTLE_CAN_ID_INDEX (0x90)


/*
 * @brief Throttle enable message (CAN frame) ID.
 *
 */
#define OSCC_THROTTLE_ENABLE_CAN_ID (0x90)

/*
 * @brief Throttle disable message (CAN frame) ID.
 *
 */
#define OSCC_THROTTLE_DISABLE_CAN_ID (0x91)

/*
 * @brief Throttle command message (CAN frame) ID.
 *
 */
#define OSCC_THROTTLE_COMMAND_CAN_ID (0x92)

/*
 * @brief Throttle report message (CAN frame) ID.
 *
 */
#define OSCC_THROTTLE_REPORT_CAN_ID (0x93)

/*
 * @brief Throttle report message (CAN frame) length.
 *
 */
#define OSCC_THROTTLE_REPORT_CAN_DLC (8)

/*
 * @brief Throttle report message publishing frequency. [Hz]
 *
 */
#define OSCC_REPORT_THROTTLE_PUBLISH_FREQ_IN_HZ (50)

/*
 * @brief Enumeration of all possible throttle DTCs.
 *
 */
enum
{
    /* DTC bitfield position indicating an invalid sensor value. */
    OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL = 0,

    /* DTC bitfield position indicating an operator override. */
    OSCC_THROTTLE_DTC_OPERATOR_OVERRIDE,

    /* Number of possible throttle DTCs. */
    OSCC_THROTTLE_DTC_COUNT
};


#pragma pack(push)
#pragma pack(1)

/**
 * @brief Throttle enable message.
 *
 * CAN frame ID: \ref OSCC_THROTTLE_ENABLE_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t reserved[6]; /*!< Reserved. */
} oscc_throttle_enable_s;


/**
 * @brief Throttle disable message.
 *
 * CAN frame ID: \ref OSCC_THROTTLE_DISABLE_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t reserved[6]; /*!< Reserved. */
} oscc_throttle_disable_s;


/**
 * @brief Throttle command message.
 *
 * CAN frame ID: \ref OSCC_THROTTLE_COMMAND_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    float torque_request; /* Torque request from 0.0 to 1.0 where 1.0 is 100% */

    uint8_t reserved[2]; /*!< Reserved. */
} oscc_throttle_command_s;


/**
 * @brief Throttle report message.
 *
 * CAN frame ID: \ref OSCC_THROTTLE_REPORT_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t enabled; /*!< Throttle controls enabled state.
                      * Zero value means disabled (commands are ignored).
                      * Non-zero value means enabled (commands are sent to the vehicle). */

    uint8_t operator_override; /*!< Driver override state.
                                * Zero value means there has been no operator override.
                                * Non-zero value means an operator has physically overridden
                                * the system. */

    uint8_t dtcs; /*!< Bitfield of DTCs present in the module. */

    uint8_t reserved[3]; /*!< Reserved. */
} oscc_throttle_report_s;

#pragma pack(pop)


#endif /* _OSCC_THROTTLE_CAN_PROTOCOL_H_ */
