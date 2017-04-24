#ifndef _OSCC_THROTTLE_PROTOCOL_CAN_H_
#define _OSCC_THROTTLE_PROTOCOL_CAN_H_


/*
 * @brief Throttle command message (CAN frame) ID.
 *
 */
#define OSCC_CAN_ID_THROTTLE_COMMAND (0x062)


/*
 * @brief Throttle report message (CAN frame) ID.
 *
 */
#define OSCC_CAN_ID_THROTTLE_REPORT (0x063)


/*
 * @brief Throttle report message publishing interval.
 *
 */
#define OSCC_PUBLISH_INTERVAL_THROTTLE_REPORT (20)


/**
 * @brief Throttle command message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref OSCC_CAN_ID_THROTTLE_COMMAND
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t accelerator_command; /*!< Accelerator command. [65535 == 100%] */
    //
    //
    uint8_t reserved_0; /*!< Reserved data. */
    //
    //
    uint8_t enabled : 1; /*!< This throttle control command/request enabled.
                          * Value zero means off/disabled.
                          * Value one means on/enabled. */
    //
    //
    uint8_t clear : 1; /*!< Clear driver override flag.
                        * Value zero means do not clear (normal operation).
                        * Value one means request clear of driver override. */
    //
    //
    uint8_t ignore : 1; /*!< Ignore driver override flag.
                         * Value zero means do not ignore (normal operation).
                         * Value one means ignore the driver override. */
    //
    //
    uint8_t reserved_1 : 5; /*!< Reserved data. */
    //
    //
    uint8_t reserved_2; /*!< Reserved data. */
    //
    //
    uint8_t reserved_3; /*!< Reserved data. */
    //
    //
    uint8_t reserved_4; /*!< Reserved data. */
    //
    //
    uint8_t count; /*!< Optional watchdog counter. */
} oscc_command_msg_throttle;


/**
 * @brief Throttle report message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref OSCC_CAN_ID_THROTTLE_REPORT
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t accelerator_input; /*!< Accelerator input value from
                                 * the physical accelerator. [65535 == 100%] */
    //
    //
    uint16_t accelerator_command; /*!< Accelerator command value from
                                   * the command message. [65535 == 100%] */
    //
    //
    uint16_t accelerator_output; /*!< Accelerator output value.
                                  * Set the maximum of
                                  * accelerator_input and accelerator command. [65535 == 100%] */
    //
    //
    uint8_t reserved_0 : 4; /*!< Reserved data. */
    //
    //
    uint8_t wdc_source : 4; /*!< Watchdog counter source.
                             * Value \ref PS_CTRL_FAULT_SOURCE_NONE means none. */
    //
    //
    uint8_t enabled : 1; /*!< Throttle controls enabled state.
                          * Value zero means off/disabled (commands are ignored).
                          * Value one means on/enabled (no timeouts or overrides have occured). */
    //
    //
    uint8_t override : 1; /*!< Driver override state.
                           * Value zero means controls are provided autonomously (no overrride).
                           * Value one means controls are provided by the driver. */
    //
    //
    uint8_t driver_activity : 1; /*!< Driver activity.
                                  * Value zero means no activity.
                                  * Value one means driver activity. */
    //
    //
    uint8_t fault_wdc : 1; /*!< Watchdog counter fault state.
                            * Value zero means no fault.
                            * Value one means fault active. */
    //
    //
    uint8_t fault_1 : 1; /*!< Channel 1 fault state.
                          * Value zero means no fault.
                          * Value one means fault active. */
    //
    //
    uint8_t fault_2 : 1; /*!< Channel 2 fault state.
                          * Value zero means no fault.
                          * Value one means fault active. */
    //
    //
    uint8_t reserved_1 : 1; /*!< Reserved data. */
    //
    //
    uint8_t fault_connector : 1; /*!< Connector fault state.
                                  * Value zero means no fault (CD pins shorted).
                                  * Value one means fault active (CD pins not shorted). */
} oscc_report_msg_throttle;


#endif /* _OSCC_THROTTLE_PROTOCOL_CAN_H_ */
