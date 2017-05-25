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
#include "oscc_pid.h"

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

/*
 * @brief Windup guard of the PID controller.
 *
 */
#define PID_WINDUP_GUARD ( 1500 )

/*
 *
 * @brief Time between steering control updates (operator override checks and
 *        steering output updates).
 */
 #define CONTROL_LOOP_INTERVAL_IN_MSEC ( 50 )


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
EXTERN uint32_t g_chassis_state_1_report_last_rx_timestamp;
EXTERN uint32_t g_last_control_loop_timestamp;

EXTERN kia_soul_steering_control_state_s g_steering_control_state;

EXTERN pid_s g_pid;
EXTERN uint16_t g_spoofed_torque_output_sum;


#endif
