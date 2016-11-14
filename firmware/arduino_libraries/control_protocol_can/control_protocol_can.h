/************************************************************************/
/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* This file is part of Open Source Car Control (OSCC).                 */
/*                                                                      */
/* OSCC is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* OSCC is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with OSCC.  If not, see <http://www.gnu.org/licenses/>.        */
/************************************************************************/

/**
 * @file control_protocol_can.h
 * @brief PolySync Vehicle Control CAN Protocol.
 *
 */


#ifndef CONTROL_PROTOCOL_CAN_H
#define	CONTROL_PROTOCOL_CAN_H


#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>


/*
 * @brief Brake command message (CAN frame) ID.
 *
 */
#define PS_CTRL_MSG_ID_BRAKE_COMMAND (0x060)


/*
 * @brief Brake report message (CAN frame) ID.
 *  
 */
#define PS_CTRL_MSG_ID_BRAKE_REPORT (0x061)


// ms
#define PS_CTRL_BRAKE_REPORT_PUBLISH_INTERVAL (50)


/*
 * @brief Throttle command message (CAN frame) ID.
 *  
 */
#define PS_CTRL_THROTTLE_COMMAND_ID (0x062)


/*
 * @brief Throttle report message (CAN frame) ID.
 *  
 */
#define PS_CTRL_MSG_ID_THROTTLE_REPORT (0x063)


//
#define PS_CTRL_THROTTLE_REPORT_PUBLISH_INTERVAL (20)


/*
 * @brief Steering command message (CAN frame) ID.
 *
 */
#define PS_CTRL_MSG_ID_STEERING_COMMAND (0x064)


/*
 * @brief Steering report message (CAN frame) ID.
 *
 */
#define PS_CTRL_MSG_ID_STEERING_REPORT (0x065)


//
#define PS_CTRL_STEERING_REPORT_PUBLISH_INTERVAL (20)


//
#define PSVC_HEARTBEAT_MSG_BASE_ID (0x100)


//
#define PSVC_HEARTBEAT_MSG_DLC (8)


// ms
#define PSVC_HEARTBEAT_MSG_TX_PUBLISH_INTERVAL (50)


//
#define PSVC_CHASSIS_STATE1_MSG_ID (0x210)


//
#define PSVC_CHASSIS_STATE1_MSG_DLC (8)


// ms
#define PSVC_CHASSIS_STATE1_MSG_TX_PUBLISH_INTERVAL (50)


//
#define PSVC_CHASSIS_STATE2_MSG_ID (0x211)


//
#define PSVC_CHASSIS_STATE2_MSG_DLC (8)


// ms
#define PSVC_CHASSIS_STATE2_MSG_TX_PUBLISH_INTERVAL (50)


//
#define PSVC_HEARTBEAT_STATE_INVALID (0x00)


//
#define PSVC_HEARTBEAT_STATE_INIT (0x01)


//
#define PSVC_HEARTBEAT_STATE_OK (0x02)


//
#define PSVC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT (0x0100)


//
#define PSVC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT (0x0200)


//
#define PSVC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT (0x0400)


//
#define PSVC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT (0x0800)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_VALID (0x02)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_STEER_WHEEL_ANGLE_RATE_VALID (0x04)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_PRESSURE_VALID (0x08)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_WHEEL_SPEED_VALID (0x10)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_LEFT_TURN_SIGNAL_ON (0x20)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_RIGHT_TURN_SIGNAL_ON (0x40)


//
#define PSVC_CHASSIS_STATE1_FLAG_BIT_BRAKE_SIGNAL_ON (0x80)

//
#define KIA_STATUS1_MESSAGE_ID (0x210)



//
typedef struct
{
    //
    //
    uint8_t hardware_version : 4;
    //
    //
    uint8_t firmware_version : 4;
    //
    //
    uint8_t state;
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t error_register;
    //
    //
    uint16_t warning_register;
} psvc_heartbeat_data_s;



//
typedef struct
{
    //
    // valid flags, signal states, etc
    uint8_t flags;
    //
    //
    uint8_t reserved_0;
    //
    //
    int16_t steering_wheel_angle;
    //
    //
    int16_t steering_wheel_angle_rate;
    //
    //
    int16_t brake_pressure;
} psvc_chassis_state1_data_s;



//
typedef struct
{
    //
    //
    int16_t wheel_speed_lf;
    //
    //
    int16_t wheel_speed_rf;
    //
    //
    int16_t wheel_speed_lr;
    //
    //
    int16_t wheel_speed_rr;
} psvc_chassis_state2_data_s;



/**
 * @brief Brake command message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_BRAKE_COMMAND
 * Receive rate: 20 ms
 * Receive timeout: 100 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t pedal_command; /*!< Pedal command. [65535 == 100%] */
    //
    //
    uint8_t brake_on : 1; /*!< Brake on-off command.
                           * Value zero means off/disabled.
                           * Value one means on/enabled. */
    //
    //
    uint8_t reserved_0 : 7; /*!< Reserved data. */
    //
    //
    uint8_t enabled : 1; /*!< This brake control command/request enabled.
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
} ps_ctrl_brake_command_msg;



/**
 * @brief Brake report message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_BRAKE_REPORT
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t pedal_input; /*!< Pedal input value from
                           * the physical pedal. [65535 == 100%] */
    //
    //
    uint16_t pedal_command; /*!< Pedal command value from
                             * the command message. [65535 == 100%] */
    //
    //
    uint16_t pedal_output; /*!< Pedal output value.
                            * Set to the maximum of
                            * pedal_input and pedal command. [65535 == 100%] */
    //
    //
    uint8_t brake_on_output : 1; /*!< Brake on-off output state.
                                  * Value zero means off.
                                  * Value one means on. */
    //
    //
    uint8_t brake_on_command : 1; /*!< Brake on-off command state.
                                   * Value zero means off.
                                   * Value one means on. */
    //
    //
    uint8_t brake_on_input : 1; /*!< Brake on-off input state.
                                 * Value zero means off.
                                 * Value one means on. */
    //
    //
    uint8_t wdc_brake : 1; /*!< Watchdog counter is applying brakes.
                            * Value zero means off.
                            * Value one means on. */
    //
    //
    uint8_t wdc_source : 4; /*!< Watchdog counter source.
                             * Value \ref PS_CTRL_FAULT_SOURCE_NONE means none. */
    //
    //
    uint8_t enabled : 1; /*!< Brake controls enabled state.
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
    uint8_t fault_brake : 1; /*!< Brake on-off connector fault state.
                              * Value zero means no fault.
                              * Value one means fault active. */
    //
    //
    uint8_t fault_connector : 1; /*!< Connector fault state.
                                  * Value zero means no fault (CD pins shorted).
                                  * Value one means fault active (CD pins not shorted). */
} ps_ctrl_brake_report_msg;



/**
 * @brief Throttle command message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_THROTTLE_COMMAND
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t pedal_command; /*!< Pedal command. [65535 == 100%] */
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
} ps_ctrl_throttle_command_msg;



/**
 * @brief Throttle report message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_THROTTLE_REPORT
 * Transmit rate: 20 ms
 *
 */
typedef struct
{
    //
    //
    uint16_t pedal_input; /*!< Pedal input value from
                           * the physical pedal. [65535 == 100%] */
    //
    //
    uint16_t pedal_command; /*!< Pedal command value from
                             * the command message. [65535 == 100%] */
    //
    //
    uint16_t pedal_output; /*!< Pedal output value.
                            * Set the maximum of
                            * pedal_input and pedal command. [65535 == 100%] */
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
} ps_ctrl_throttle_report_msg;



/**
 * @brief Steering command message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_STEERING_COMMAND
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
} ps_ctrl_steering_command_msg;



/**
 * @brief Steering report message.
 *
 * Message size (CAN frame DLC): 8 bytes
 * CAN frame ID: \ref PS_CTRL_MSG_ID_STEERING_REPORT
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
} ps_ctrl_steering_report_msg;




#ifdef __cplusplus
}
#endif


#endif	/* CONTROL_PROTOCOL_CAN_H */
