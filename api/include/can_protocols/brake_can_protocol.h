/**
 * @file brake_can_protocol.h
 * @brief Brake CAN Protocol.
 *
 */


#ifndef _OSCC_BRAKE_CAN_PROTOCOL_H_
#define _OSCC_BRAKE_CAN_PROTOCOL_H_


#include <stdint.h>
#include "magic.h"


/*
 * @brief CAN ID representing the range of brake messages.
 *
 */
#define OSCC_BRAKE_CAN_ID_INDEX (0x70)


/*
 * @brief Brake enable message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_ENABLE_CAN_ID (0x70)

/*
 * @brief Brake disable message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_DISABLE_CAN_ID (0x71)

/*
 * @brief Brake command message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_COMMAND_CAN_ID (0x72)

/*
 * @brief Brake report message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_REPORT_CAN_ID (0x73)

/*
 * @brief Brake report message (CAN frame) length.
 *
 */
#define OSCC_BRAKE_REPORT_CAN_DLC (8)

/*
 * @brief Brake report message publishing frequency. [Hz]
 *
 */
#define OSCC_BRAKE_REPORT_PUBLISH_FREQ_IN_HZ (50)

/*
 * @brief Enumeration of all possible brake DTCs.
 *
 */
enum
{
    /* DTC bitfield position indicating an invalid sensor value. */
    OSCC_BRAKE_DTC_INVALID_SENSOR_VAL = 0,

    /* DTC bitfield position indicating an operator override. */
    OSCC_BRAKE_DTC_OPERATOR_OVERRIDE,

    /* Number of possible brake DTCs. */
    OSCC_BRAKE_DTC_COUNT
};


#pragma pack(push)
#pragma pack(1)

/**
 * @brief Brake enable message.
 *
 * CAN frame ID: \ref OSCC_BRAKE_ENABLE_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t reserved[6]; /*!< Reserved. */
} oscc_brake_enable_s;


/**
 * @brief Brake disable message.
 *
 * CAN frame ID: \ref OSCC_BRAKE_DISABLE_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t reserved[6]; /*!< Reserved. */
} oscc_brake_disable_s;


/**
 * @brief Brake command message data.
 *
 * CAN frame ID: \ref OSCC_BRAKE_COMMAND_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    float pedal_command; /* Brake Request 0.0 to 1.0 where 1.0 is 100% */

    uint8_t reserved[2]; /*!< Reserved. */

} oscc_brake_command_s;


/**
 * @brief Brake report message data.
 *
 * CAN frame ID: \ref OSCC_BRAKE_REPORT_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint8_t enabled; /*!< Braking controls enabled state.
                      * Zero value means disabled (commands are ignored).
                      * Non-zero value means enabled (no timeouts or overrides have occured). */

    uint8_t operator_override; /*!< Driver override state.
                                * Zero value means there has been no operator override.
                                * Non-zero value means an operator has physically overridden
                                * the system. */

    uint8_t dtcs; /*!< Bitfield of DTCs present in the module. */

    uint8_t reserved[3]; /*!< Reserved. */
} oscc_brake_report_s;

#pragma pack(pop)

#endif /* _OSCC_BRAKE_CAN_PROTOCOL_H_ */
