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
 * @brief Brake enable message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_ENABLE_CAN_ID (0x50)

/*
 * @brief Brake disable message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_DISABLE_CAN_ID (0x51)

/*
 * @brief Brake command message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_COMMAND_CAN_ID (0x60)

/*
 * @brief Brake report message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_REPORT_CAN_ID (0x61)

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

#if defined(KIA_SOUL)
    uint16_t pedal_command; /*!< Pedal command. [65535 == 100%] */

    uint8_t reserved[4]; /*!< Reserved. */
#elif defined(KIA_SOUL_EV)
    uint16_t spoof_value_low; /*!< Value to be sent on the low spoof signal. */

    uint16_t spoof_value_high; /*!< Value to be sent on the high spoof signal. */

    uint8_t reserved[2]; /*!< Reserved. */
#endif
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
