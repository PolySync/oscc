#ifndef _OSCC_STEERING_PROTOCOL_CAN_H_
#define _OSCC_STEERING_PROTOCOL_CAN_H_


#include <stdint.h>


/*
 * @brief Steering command message (CAN frame) ID.
 *
 */
#define OSCC_COMMAND_STEERING_CAN_ID (0x64)


/*
 * @brief Steering report message (CAN frame) ID.
 *
 */
#define OSCC_REPORT_STEERING_CAN_ID (0x65)


/*
 * @brief Steering report message (CAN frame) length.
 *
 */
#define OSCC_REPORT_STEERING_CAN_DLC (8)


/*
 * @brief Steering report message publishing interval.
 *
 */
#define OSCC_REPORT_STEERING_PUBLISH_INTERVAL_IN_MSEC (20)


/**
 * @brief Steering command message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref OSCC_COMMAND_STEERING_CAN_ID
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    int16_t steering_wheel_angle_command; /*!< Steering wheel angle command.
                                           * Positive means to the
                                           * left (counter clockwise). [0.1 degrees per bit] */
    //
    //
    uint8_t enabled : 1; /*!< This steering control command/request enabled.
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
    uint8_t reserved_0 : 5; /*!< Reserved data. */
    //
    //
    uint8_t steering_wheel_max_velocity; /*!< Steering wheel
                                          * maximum velocity command.
                                          * Value zero means no limit.
                                          * Value 0x01 means 2 degrees/second.
                                          * Value 0xFA means 500 degrees/second. [2 degrees/second per bit] */
    //
    //
    uint16_t torque; /*!< Reserved data. */
    //
    //
    uint8_t reserved_3; /*!< Reserved data. */
    //
    //
    uint8_t count; /*!< Optional watchdog counter. */
} oscc_command_steering_data_s;


typedef struct
{
    uint32_t timestamp;
    oscc_command_steering_data_s data;
} oscc_command_steering_s;


/**
 * @brief Steering report message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref OSCC_REPORT_STEERING_CAN_ID
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    int16_t angle; /*!< Steering wheel angle.
                    * Positive means to the
                    * left (counter clockwise). [0.1 degrees per bit] */
    //
    //
    int16_t angle_command; /*!< Steering wheel angle command.
                            * Positive means to the
                            * left (counter clockwise). [0.1 degrees per bit] */
    //
    //
    uint16_t vehicle_speed; /*!< Vehicle speed. [0.01 kilometers/hour per bit] */
    //
    //
    int8_t torque; /*!< Steering wheel torque. [0.0625 Newton meters per bit] */
    //
    //
    uint8_t enabled : 1; /*!< Steering controls enabled state.
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
    uint8_t fault_calibration : 1; /*!< Calibration fault state.
                                    * Value zero means no fault.
                                    * Value one means fault active. */
    //
    //
    uint8_t fault_connector : 1; /*!< Connector fault state.
                                  * Value zero means no fault (CD pins shorted).
                                  * Value one means fault active (CD pins not shorted). */
} oscc_report_steering_data_s;


typedef struct
{
    const uint32_t id = OSCC_REPORT_STEERING_CAN_ID;
    const uint8_t dlc = OSCC_REPORT_STEERING_CAN_DLC;
    uint32_t timestamp;
    oscc_report_steering_data_s data;
} oscc_report_steering_s;


#endif /* _OSCC_STEERING_PROTOCOL_CAN_H_ */
