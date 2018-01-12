/**
 * @file globals.h
 * @brief Module globals.
 *
 */


#ifndef _OSCC_BRAKE_GLOBALS_H_
#define _OSCC_BRAKE_GLOBALS_H_


#include "brake_control.h"
#include "mcp_can.h"
#include "oscc_pid.h"


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
 * @brief Pin of the accumulator pump motor check (MTT) signal.
 *
 */
#define PIN_ACCUMULATOR_PUMP_MOTOR_CHECK ( 8 )

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
 * @brief Pin of the front left pressure sensor.
 *
 */
#define PIN_PRESSURE_SENSOR_FRONT_LEFT ( 14 )

/*
 * @brief Pin of the front right pressure sensor.
 *
 */
#define PIN_PRESSURE_SENSOR_FRONT_RIGHT ( 13 )

/*
 * @brief Pin of the wheel pressure check 1 (PCK1) signal.
 *
 */
#define PIN_WHEEL_PRESSURE_CHECK_1 ( 13 )

/*
 * @brief Pin of the wheel pressure check 2 (PCK2) signal.
 *
 */
#define PIN_WHEEL_PRESSURE_CHECK_2 ( 12 )

/*
 * @brief Pin of the brake light.
 *
 */
#define PIN_BRAKE_LIGHT ( 48 )

/*
 * @brief PWM value to turn a solenoid off.
 *
 */
#define SOLENOID_PWM_OFF ( 0 )

/*
 * @brief PWM value to turn a solenoid on.
 *
 */
#define SOLENOID_PWM_ON ( 255 )


typedef struct
{
    uint16_t brake_pressure_sensor_check_value_min;
    uint16_t brake_pressure_sensor_check_value_max;
    uint16_t fault_check_frequency_in_hz;
    uint16_t report_publish_frequency_in_hz;
    float accumulator_pressure_min_in_decibars;
    float accumulator_pressure_max_in_decibars;
    float override_pedal_threshold_in_decibars;
    float brake_light_pressure_threshold_in_decibars;
    float brake_pressure_min_in_decibars;
    float brake_pressure_max_in_decibars;
    float pid_proportional_gain;
    float pid_integral_gain;
    float pid_derivative_gain;
    float pid_windup_guard;
    float pid_output_min;
    float pid_output_max;
    float pid_accumulator_solenoid_clamped_min;
    float pid_accumulator_solenoid_clamped_max;
    float pid_release_solenoid_clamped_min;
    float pid_release_solenoid_clamped_max;
    float accumulator_solenoid_duty_cycle_min;
    float accumulator_solenoid_duty_cycle_max;
    float release_solenoid_duty_cycle_min;
    float release_solenoid_duty_cycle_max;
} eeprom_config_s;


#ifdef GLOBAL_DEFINED
    MCP_CAN g_control_can( PIN_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN g_control_can;

    #define EXTERN extern
#endif


EXTERN volatile bool g_brake_command_timeout;

EXTERN volatile brake_control_state_s g_brake_control_state;

EXTERN pid_s g_pid;

EXTERN eeprom_config_s g_eeprom_config;


#endif /* _OSCC_BRAKE_GLOBALS_H_ */
