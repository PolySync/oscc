/**
 * @file brake_can_protocol.h
 * @brief Brake CAN Protocol.
 *
 */


#ifndef _OSCC_BRAKE_CAN_PROTOCOL_H_
#define _OSCC_BRAKE_CAN_PROTOCOL_H_


#include <stdint.h>


/*
 * @brief Brake command message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_COMMAND_CAN_ID (0x060)

/*
 * @brief Brake report message (CAN frame) ID.
 *
 */
#define OSCC_BRAKE_REPORT_CAN_ID (0x061)

/*
 * @brief Brake report message (CAN frame) length.
 *
 */
#define OSCC_BRAKE_REPORT_CAN_DLC (8)

/*
 * @brief Brake report message publishing frequency. [Hz]
 *
 */
#define OSCC_REPORT_BRAKE_PUBLISH_FREQ_IN_HZ (50)


/**
 * @brief Brake command message data.
 *
 * Message size (CAN frame DLC): 8 bytes
 *
 */
typedef struct
{
    uint16_t pedal_command; /*!< Pedal command. [65535 == 100%] */

    uint8_t reserved_0 : 1; /*!< Reserved. */

    uint8_t reserved_1 : 7; /*!< Reserved. */

    uint8_t enabled : 1; /*!< This brake control command/request enabled.
                          * Value zero means off/disabled.
                          * Value one means on/enabled. */

    uint8_t reserved_2 : 1; /*!< Reserved. */

    uint8_t reserved_3 : 1; /*!< Reserved. */

    uint8_t reserved_4 : 5; /*!< Reserved. */

    uint8_t reserved_5; /*!< Reserved. */

    uint8_t reserved_6; /*!< Reserved. */

    uint8_t reserved_7; /*!< Reserved. */

    uint8_t reserved_8; /*!< Reserved. */
} oscc_brake_command_s;


/**
 * @brief Brake report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_BRAKE_REPORT_CAN_DLC
 *
 */
typedef struct
{
    int16_t pedal_input; /*!< Pedal input value from
                           * the physical pedal. [65535 == 100%] */

    uint16_t pedal_command; /*!< Pedal command value from
                             * the command message. [65535 == 100%] */

    uint16_t pedal_output; /*!< Pedal output value.
                            * Set to the maximum of
                            * pedal_input and pedal command. [65535 == 100%] */

    uint8_t reserved_0 : 1; /*!< Reserved. */

    uint8_t reserved_1 : 1; /*!< Reserved. */

    uint8_t reserved_2 : 1; /*!< Reserved. */

    uint8_t reserved_3 : 1; /*!< Reserved. */

    uint8_t reserved_4 : 4; /*!< Reserved. */

    uint8_t enabled : 1; /*!< Brake controls enabled state.
                          * Value zero means off/disabled (commands are ignored).
                          * Value one means on/enabled (no timeouts or overrides have occured). */

    uint8_t override : 1; /*!< Driver override state.
                           * Value zero means controls are provided autonomously (no override).
                           * Value one means controls are provided by the driver. */

    uint8_t fault_invalid_sensor_value : 1; /*!< Invalid sensor value indicator.
                                             * Value zero means the values read
                                             * from the sensors are valid.
                                             *
                                             * Value one means the values read
                                             * from the sensors are invalid. */

    uint8_t reserved_5 : 1; /*!< Reserved. */

    uint8_t fault_obd_timeout : 1; /*!< OBD timeout indicator.
                                   * Value zero means no timeout occurred.
                                   * Value one means timeout occurred. */

    uint8_t reserved_6 : 1; /*!< Reserved */

    uint8_t reserved_7 : 1; /*!< Reserved. */

    uint8_t reserved_8 : 1; /*!< Reserved. */
} oscc_brake_report_s;


#endif /* _OSCC_BRAKE_CAN_PROTOCOL_H_ */
