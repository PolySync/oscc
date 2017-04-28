/**
 * @file brake_can_protocol.h
 * @brief Brake CAN Protocol.
 *
 */


#ifndef _OSCC_BRAKE_CAN_PROTOCOL_H_
#define _OSCC_BRAKE_CAN_PROTOCOL_H_


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


/**
 * @brief Brake command message data.
 *
 * Message size (CAN frame DLC): 8 bytes
 *
 */
typedef struct
{
    uint16_t pedal_command; /*!< Pedal command. [65535 == 100%] */

    uint8_t brake_on : 1; /*!< Brake on-off command.
                           * Value zero means off/disabled.
                           * Value one means on/enabled. */

    uint8_t reserved_0 : 7; /*!< Reserved. */

    uint8_t enabled : 1; /*!< This brake control command/request enabled.
                          * Value zero means off/disabled.
                          * Value one means on/enabled. */

    uint8_t clear : 1; /*!< Clear driver override flag.
                        * Value zero means do not clear (normal operation).
                        * Value one means request clear of driver override. */

    uint8_t ignore : 1; /*!< Ignore driver override flag.
                         * Value zero means do not ignore (normal operation).
                         * Value one means ignore the driver override. */

    uint8_t reserved_1 : 5; /*!< Reserved. */

    uint8_t reserved_2; /*!< Reserved. */

    uint8_t reserved_3; /*!< Reserved. */

    uint8_t reserved_4; /*!< Reserved. */

    uint8_t count; /*!< Optional watchdog counter. */
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
    uint16_t pedal_input; /*!< Pedal input value from
                           * the physical pedal. [65535 == 100%] */

    uint16_t pedal_command; /*!< Pedal command value from
                             * the command message. [65535 == 100%] */

    uint16_t pedal_output; /*!< Pedal output value.
                            * Set to the maximum of
                            * pedal_input and pedal command. [65535 == 100%] */

    uint8_t brake_on_output : 1; /*!< Brake on-off output state.
                                  * Value zero means off.
                                  * Value one means on. */

    uint8_t brake_on_command : 1; /*!< Brake on-off command state.
                                   * Value zero means off.
                                   * Value one means on. */

    uint8_t brake_on_input : 1; /*!< Brake on-off input state.
                                 * Value zero means off.
                                 * Value one means on. */

    uint8_t wdc_brake : 1; /*!< Watchdog counter is applying brakes.
                            * Value zero means off.
                            * Value one means on. */

    uint8_t wdc_source : 4; /*!< Watchdog counter source.
                             * Value \ref PS_CTRL_FAULT_SOURCE_NONE means none. */

    uint8_t enabled : 1; /*!< Brake controls enabled state.
                          * Value zero means off/disabled (commands are ignored).
                          * Value one means on/enabled (no timeouts or overrides have occured). */

    uint8_t override : 1; /*!< Driver override state.
                           * Value zero means controls are provided autonomously (no override).
                           * Value one means controls are provided by the driver. */

    uint8_t driver_activity : 1; /*!< Driver activity.
                                  * Value zero means no activity.
                                  * Value one means driver activity. */

    uint8_t fault_wdc : 1; /*!< Watchdog counter fault state.
                            * Value zero means no fault.
                            * Value one means fault active. */

    uint8_t fault_1 : 1; /*!< Channel 1 fault state.
                          * Value zero means no fault.
                          * Value one means fault active. */

    uint8_t fault_2 : 1; /*!< Channel 2 fault state.
                          * Value zero means no fault.
                          * Value one means fault active. */

    uint8_t fault_brake : 1; /*!< Brake on-off connector fault state.
                              * Value zero means no fault.
                              * Value one means fault active. */

    uint8_t fault_connector : 1; /*!< Connector fault state.
                                  * Value zero means no fault (CD pins shorted).
                                  * Value one means fault active (CD pins not shorted). */
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
