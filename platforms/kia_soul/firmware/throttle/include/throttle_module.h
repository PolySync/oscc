#ifndef _OSCC_KIA_SOUL_THROTTLE_MODULE_H_
#define _OSCC_KIA_SOUL_THROTTLE_MODULE_H_

#include <stdint.h>


/**
 * @brief Override flags.
 *
 * Tracks override flags for accelerator and voltages.
 *
 */
typedef struct
{
    bool accelerator_pressed; /* Tracks whether accelerator is pressed */
    uint16_t voltage; /* Tracks any DAC/ADC voltage discrepancies */
    uint16_t voltage_spike_a; /* Used to filter any DAC/ADC voltage spikes */
    uint16_t voltage_spike_b; /* Used to filter any DAC/ADC voltage spikes */
} kia_soul_throttle_override_flags_s;


/**
 * @brief Current throttle state.
 *
 * Keeps track of the current state of the throttle system.
 *
 */
typedef struct
{
    uint16_t accel_pos_sensor_high; /* Value of high signal of accelerator position sensor */
    uint16_t accel_pos_sensor_low; /* Value of low signal of accelerator position sensor */
    float accel_pos_target; /* As specified by higher level controller */
} kia_soul_throttle_state_s;


/**
 * @brief Current throttle control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_throttle_control_state_s;


#endif
