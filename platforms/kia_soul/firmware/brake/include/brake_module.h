#ifndef _OSCC_KIA_SOUL_BRAKE_MODULE_H_
#define _OSCC_KIA_SOUL_BRAKE_MODULE_H_


typedef struct
{
    float pressure;
} kia_soul_accumulator_s;


typedef struct
{
    // All pressures are in tenths of a bar (decibars) to match the values coming from the vehicle
    const float min_accumulator_pressure = 777.6; // min pressure to maintain (decibars)
    const float max_accumulator_pressure = 878.3; // max pressure to maintain (decibars)
    const float min_brake_pressure = 12.0;
    const float max_brake_pressure = 947.9;
    const float driver_override_pedal_threshold = 43.2; // Pedal pressure for driver override
    const uint16_t rx_timeout = 2500; /* Amount of time when system is considered unresponsive (milliseconds) */
    const float pid_proportional_gain = 0.5;
    const float pid_integral_gain = 0.2;
    const float pid_derivative_gain = 0.001;
    const uint8_t pid_windup_guard = 30;

    // The min/max duty cycle scalars used for 3.921 KHz PWM frequency.
    // These represent the minimum duty cycles that begin to actuate the
    // proportional solenoids and the maximum duty cycle where the solenoids
    // have reached their stops.
    const float sla_duty_cycle_max = 105.0;
    const float sla_duty_cycle_min = 80.0;
    const float slr_duty_cycle_max = 100.0;
    const float slr_duty_cycle_min = 50.0;
} kia_soul_brake_params_s;


typedef struct
{
    const uint8_t can_cs = 53; /* CAN chip select */

    // Pins are not perfectly sequential because the clock frequency of certain
    // pins are different

    // Duty cycles of pins 3 and 5 controlled by timer 3 (TCCR3B)
    const uint8_t slafl = 5; // front left accumulator solenoid
    const uint8_t slafr = 7; // front right accumulator solenoid

    // Duty cycles of pins 6, 7, and 8 controlled by timer 4 (TCCR4B)
    const uint8_t slrfl = 6; // front left release solenoid
    const uint8_t slrfr = 8; // front right release solenoid
    const uint8_t smc = 2; // master cylinder solenoids

    // Digital pin controls
    const uint8_t brake_light = 48; // Tail light control
    const uint8_t accumulator_pump = 49; // accumulator pump motor

    // Analog sensor pin definitions
    const uint8_t pacc = 9; // accumulator pressure sensor
    const uint8_t pmc1 = 10; // master cylinder pressure sensor 1
    const uint8_t pmc2 = 11; // master cylinder pressure sensor 2
    const uint8_t pfr = 13; // front right pressure sensor
    const uint8_t pfl = 14; // front left pressure sensor
} kia_soul_brake_pins_s;


/**
 * @brief Current brake state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    kia_soul_accumulator_s accumulator;
    float current_pressure;
    float can_pressure;
    uint16_t pedal_command;
} kia_soul_brake_state_s;


/**
 * @brief Current brake control state.
 *
 * Keeps track of what control state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled;
    bool enable_request;
    uint32_t rx_timestamp;
    int16_t driver_override;
} kia_soul_brake_control_state_s;


typedef struct
{
    kia_soul_brake_state_s state; /* State of the brake system */
    kia_soul_brake_control_state_s control_state; /* Control state of the brake system */
    kia_soul_brake_params_s params; /* Parameters of the brake system */
    kia_soul_brake_pins_s pins; /* Pin assignments of the sensor interface board */
} kia_soul_brake_module_s;

#endif
