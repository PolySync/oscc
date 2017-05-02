/**
 * @file globals.h
 * @brief Module globals.
 *
 */


#ifndef _KIA_SOUL_STEERING_GLOBALS_H_
#define _KIA_SOUL_STEERING_GLOBALS_H_


#include <stdint.h>
#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "pid.h"

#include "steering_control.h"


/*
 * @brief Chip select pin of the DAC IC.
 *
 */
#define PIN_DAC_CHIP_SELECT ( 9 )

/*
 * @brief Chip select pin of the CAN IC.
 *
 */
#define PIN_CAN_CHIP_SELECT ( 10 )

/*
 * @brief High signal pin of the torque sensor.
 *
 */
#define PIN_TORQUE_SENSOR_HIGH ( A0 )

/*
 * @brief Low signal pin of the torque sensor.
 *
 */
#define PIN_TORQUE_SENSOR_LOW ( A1 )

/*
 * @brief High signal pin of the torque spoof output.
 *
 */
#define PIN_TORQUE_SPOOF_HIGH ( A2 )

/*
 * @brief Low signal pin of the torque spoof output.
 *
 */
#define PIN_TORQUE_SPOOF_LOW ( A3 )

/*
 * @brief Relay enable pin for the spoof output.
 *
 */
#define PIN_SPOOF_ENABLE ( 6 )


/*******************************************************************************
*   WARNING
*
*   The ranges selected to do steering control are carefully tested to
*   ensure that a torque is not requested that the vehicles steering motor
*   cannot handle. By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/

/*
 * @brief Minimum steering angle rate. [degrees/microsecond]
 *
 */
#define PARAM_STEERING_ANGLE_RATE_MIN_IN_DEGREES_PER_USEC ( -1000.0 )

/*
 * @brief Maximum steering angle rate. [degrees/microsecond]
 *
 */
#define PARAM_STEERING_ANGLE_RATE_MAX_IN_DEGREES_PER_USEC ( 1000.0 )

/*
 * @brief Minimum steering angle rate. [Newton meters]
 *
 */
#define PARAM_TORQUE_MIN_IN_NEWTON_METERS ( -1500.0 )

/*
 * @brief Maximum steering angle rate. [Newton meters]
 *
 */
#define PARAM_TORQUE_MAX_IN_NEWTON_METERS ( 1500.0 )

/*
 * @brief Value of the torque sensor that indicates operator override.
          [degrees/microsecond]
 *
 */
#define PARAM_OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC ( 3000 )

/*
 * @brief Amount of time after controller command that is considered a
 *        timeout. [milliseconds]
 *
 */
#define PARAM_COMMAND_TIMEOUT_IN_MSEC ( 250 )

/*
 * @brief Proportional gain of the PID controller.
 *
 */
#define PARAM_PID_PROPORTIONAL_GAIN ( 0.3 )

/*
 * @brief Integral gain of the PID controller.
 *
 */
#define PARAM_PID_INTEGRAL_GAIN ( 1.3 )

/*
 * @brief Derivative gain of the PID controller.
 *
 */
#define PARAM_PID_DERIVATIVE_GAIN ( 0.03 )

/*
 * @brief Windup guard of the PID controller.
 *
 */
#define PARAM_PID_WINDUP_GUARD ( 1500 )


#ifdef GLOBAL_DEFINED
    DAC_MCP49xx g_dac( DAC_MCP49xx::MCP4922, PIN_DAC_CHIP_SELECT );
    MCP_CAN g_control_can( PIN_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern DAC_MCP49xx g_dac;
    extern MCP_CAN g_control_can;

    #define EXTERN extern
#endif


EXTERN uint32_t g_steering_command_last_rx_timestamp;
EXTERN uint32_t g_steering_report_last_tx_timestamp;

EXTERN kia_soul_steering_control_state_s g_steering_control_state;

EXTERN pid_s g_pid;
EXTERN uint8_t g_torque_sum;


#endif
