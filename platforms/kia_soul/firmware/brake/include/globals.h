#ifndef _OSCC_KIA_SOUL_BRAKE_GLOBALS_H_
#define _OSCC_KIA_SOUL_BRAKE_GLOBALS_H_


#include "mcp_can.h"

#include "brake_control.h"


#define PARAM_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS ( 777.6 )
#define PARAM_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS ( 878.3 )
#define PARAM_BRAKE_PRESSURE_MIN_IN_DECIBARS ( 12.0 )
#define PARAM_BRAKE_PRESSURE_MAX_IN_DECIBARS ( 947.9 )
#define PARAM_DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ( 43.2 )
#define PARAM_COMMAND_TIMEOUT_IN_MSEC ( 2500 )
#define PARAM_PID_PROPORTIONAL_GAIN ( 0.5 )
#define PARAM_PID_INTEGRAL_GAIN ( 0.2 )
#define PARAM_PID_DERIVATIVE_GAIN ( 0.001 )
#define PARAM_PID_WINDUP_GUARD ( 30 )

// The min/max duty cycle scalars used for 3.921 KHz PWM frequency.
// These represent the minimum duty cycles that begin to actuate the
// proportional solenoids and the maximum duty cycle where the solenoids
// have reached their stops.
#define PARAM_SLA_DUTY_CYCLE_MAX ( 105.0 )
#define PARAM_SLA_DUTY_CYCLE_MIN ( 80.0 )
#define PARAM_SLR_DUTY_CYCLE_MAX ( 100.0 )
#define PARAM_SLR_DUTY_CYCLE_MIN ( 50.0 )


// Pins are not perfectly sequential because the clock frequency of certain pins are different
// Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
// Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
#define PIN_CAN_CHIP_SELECT ( 53 )

#define PIN_ACCUMULATOR_SOLENOID_FRONT_LEFT ( 5 )
#define PIN_ACCUMULATOR_SOLENOID_FRONT_RIGHT ( 7 )
#define PIN_ACCUMULATOR_PRESSURE_SENSOR ( 9 )
#define PIN_ACCUMULATOR_PUMP_MOTOR ( 49 )
#define PIN_MASTER_CYLINDER_SOLENOID ( 2 )
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 ( 10 )
#define PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 ( 11 )
#define PIN_RELEASE_SOLENOID_FRONT_LEFT ( 6 )
#define PIN_RELEASE_SOLENOID_FRONT_RIGHT ( 8 )
#define PIN_PRESSURE_SENSOR_FRONT_RIGHT ( 13 )
#define PIN_PRESSURE_SENSOR_FRONT_LEFT ( 14 )
#define PIN_BRAKE_LIGHT ( 48 )


#ifdef GLOBAL_DEFINED
    MCP_CAN can( PIN_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN can;

    #define EXTERN extern
#endif


EXTERN uint32_t g_brake_command_last_rx_timestamp;
EXTERN uint32_t g_brake_report_last_tx_timestamp;

EXTERN kia_soul_brake_state_s brake_state;
EXTERN kia_soul_brake_control_state_s brake_control_state;


#endif
