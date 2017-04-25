#ifndef _OSCC_KIA_SOUL_THROTTLE_MODULE_H_
#define _OSCC_KIA_SOUL_THROTTLE_MODULE_H_

#include <stdint.h>


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
    bool operator_override; /* Flag indicating whether accelerator was manually pressed by operator */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_throttle_control_state_s;


#endif
