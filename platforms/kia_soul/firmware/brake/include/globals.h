/**
 * @file globals.h
 * @brief Module globals.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_GLOBALS_H_
#define _OSCC_KIA_SOUL_BRAKE_GLOBALS_H_


#include "mcp_can.h"

#include "brake_control.h"


/*
 * @brief Chip select pin of the CAN IC.
 *
 */
#define PIN_CAN_CHIP_SELECT ( 53 )

/*
 * @brief Pin of the front left accumulator solenoid.
 *
 * PWM duty cycle controlled by timer 3 (TCCR3B).
 *
 */
#define PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT ( 5 )

/*
 * @brief Pin of the front right accumulator solenoid.
 *
 * PWM duty cycle controlled by timer 4 (TCCR4B).
 *
 */
#define PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT ( 7 )

/*
 * @brief Pin of the accumulator pressure sensor.
 *
 */
#define PIN_ACCUMULATOR_PRESSURE_SENSOR ( 9 )

/*
 * @brief Pin of the accumulator pump motor.
 *
 */
#define PIN_ACCUMULATOR_PUMP_MOTOR ( 49 )

/*
 * @brief Pin of the master cylinder solenoid.
 *
 */
#define PIN_MASTER_CYLINDER_SOLENOID ( 2 )

/*
 * @brief Pin of the first master cylinder pressure sensor.
 *
 */
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 ( 10 )

/*
 * @brief Pin of the second master cylinder pressure sensor.
 *
 */
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 ( 11 )

/*
 * @brief Pin of the front left release solenoid.
 *
 * PWM duty cycle controlled by timer 4 (TCCR4B).
 *
 */
#define PIN_RELEASE_SOLENOID_FRONT_LEFT ( 6 )

/*
 * @brief Pin of the front right release solenoid.
 *
 * PWM duty cycle controlled by timer 4 (TCCR4B).
 *
 */
#define PIN_RELEASE_SOLENOID_FRONT_RIGHT ( 8 )

/*
 * @brief Pin of the front right pressure sensor.
 *
 */
#define PIN_PRESSURE_SENSOR_FRONT_LEFT ( 14 )

/*
 * @brief Pin of the front left pressure sensor.
 *
 */
#define PIN_PRESSURE_SENSOR_FRONT_RIGHT ( 13 )

/*
 * @brief Pin of the brake light.
 *
 */
#define PIN_BRAKE_LIGHT ( 48 )

/*
 * @brief Minimum accumulator presure. [decibars]
 *
 */
#define PARAM_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS ( 777.6 )

/*
 * @brief Maximum accumulator pressure. [decibars]
 *
 */
#define PARAM_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS ( 878.3 )

/*
 * @brief Minimum brake pressure. [decibars]
 *
 */
#define PARAM_BRAKE_PRESSURE_MIN_IN_DECIBARS ( 12.0 )

/*
 * @brief Maximum brake pressure. [decibars]
 *
 */
#define PARAM_BRAKE_PRESSURE_MAX_IN_DECIBARS ( 947.9 )

/*
 * @brief Value of brake pressure that indicates operator override. [decibars]
 *
 */
#define PARAM_DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ( 43.2 )

/*
 * @brief Amount of time after controller command that is considered a
 *        timeout. [milliseconds]
 *
 */
#define PARAM_COMMAND_TIMEOUT_IN_MSEC ( 2500 )

/*
 * @brief Proportional gain of the PID controller.
 *
 */
#define PARAM_PID_PROPORTIONAL_GAIN ( 0.5 )

/*
 * @brief Integral gain of the PID controller.
 *
 */
#define PARAM_PID_INTEGRAL_GAIN ( 0.2 )

/*
 * @brief Derivative gain of the PID controller.
 *
 */
#define PARAM_PID_DERIVATIVE_GAIN ( 0.001 )

/*
 * @brief Windup guard of the PID controller.
 *
 */
#define PARAM_PID_WINDUP_GUARD ( 30 )

// The min/max duty cycle scalars used for 3.921 KHz PWM frequency.
// These represent the minimum duty cycles that begin to actuate the
// proportional solenoids and the maximum duty cycle where the solenoids
// have reached their stops.

/*
 * @brief Maximum SLA duty cycle.
 *
 */
#define PARAM_SLA_DUTY_CYCLE_MAX ( 105.0 )

/*
 * @brief Minimum SLA duty cycle.
 *
 */
#define PARAM_SLA_DUTY_CYCLE_MIN ( 80.0 )

/*
 * @brief Maximum SLR duty cycle.
 *
 */
#define PARAM_SLR_DUTY_CYCLE_MAX ( 100.0 )

/*
 * @brief Minimum SLR duty cycle.
 *
 */
#define PARAM_SLR_DUTY_CYCLE_MIN ( 50.0 )


#ifdef GLOBAL_DEFINED
    MCP_CAN g_control_can( PIN_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN g_control_can;

    #define EXTERN extern
#endif


EXTERN uint32_t g_brake_command_last_rx_timestamp;
EXTERN uint32_t g_brake_report_last_tx_timestamp;

EXTERN kia_soul_brake_control_state_s g_brake_control_state;


#endif /* _OSCC_KIA_SOUL_BRAKE_GLOBALS_H_ */
