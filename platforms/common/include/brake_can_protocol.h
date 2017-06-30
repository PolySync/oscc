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
#define OSCC_COMMAND_BRAKE_CAN_ID (0x060)

/*
 * @brief Brake report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_BRAKE_CAN_ID (0x061)

/*
 * @brief Brake report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_BRAKE_CAN_DLC (8)

/*
 * @brief Brake report message publishing interval. [milliseconds]
 *
 */
#define OSCC_REPORT_BRAKE_PUBLISH_INTERVAL_IN_MSEC (50)

/*
 * @brief Number of brake module DTCs.
 *
 */
#define OSCC_BRAKE_DTC_COUNT (4)


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
} oscc_command_brake_data_s;


/**
 * @brief Brake command message.
 *
 * CAN frame ID: \ref OSCC_COMMAND_BRAKE_CAN_ID
 *
 */
typedef struct
{
    uint32_t timestamp; /* Timestamp when command was received by the firmware. */

    oscc_command_brake_data_s data; /* CAN frame data. */
} oscc_command_brake_s;


/**
 * @brief Brake report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_REPORT_BRAKE_CAN_DLC
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

    uint8_t dtc00_invalid_sensor_value : 1; /*!< Invalid sensor value indicator.
                                             * Value zero means the values read
                                             * from the sensors are valid.
                                             *
                                             * Value one means the values read
                                             * from the sensors are invalid. */

    uint8_t dtc01_obd_timeout : 1; /*!< OBD timeout indicator.
                                   * Value zero means no timeout occurred.
                                   * Value one means timeout occurred. */

    uint8_t dtc02_startup_pressure_check_error : 1; /*!< Actuator sensors report
                                                     * values that do not match
                                                     * the static values
                                                     * expected with PCK1 and
                                                     * PCK2 asserted. */

    uint8_t dtc03_startup_pump_motor_check_error : 1; /*!< Pump motor check signal
                                                       * (MTT) is not asserted
                                                       * when pump is on. */

    uint8_t reserved_5 : 1; /*!< Reserved. */

    uint8_t reserved_6 : 1; /*!< Reserved. */
} oscc_report_brake_data_s;


/**
 * @brief Brake report message.
 *
 * CAN frame ID: \ref OSCC_REPORT_BRAKE_CAN_ID
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when report was put on the bus. */

    oscc_report_brake_data_s data; /* CAN frame data. */
} oscc_report_brake_s;


#endif /* _OSCC_BRAKE_CAN_PROTOCOL_H_ */
