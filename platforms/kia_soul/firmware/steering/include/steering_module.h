#ifndef _OSCC_KIA_SOUL_STEERING_MODULE_H_
#define _OSCC_KIA_SOUL_STEERING_MODULE_H_


#include <stdint.h>


/**
 * @brief Current steering state.
 *
 * Keeps track of the current state of the steering system.
 *
 */
typedef struct
{
    float steering_angle; /* Current steering angle as reported by car */
    float steering_angle_rate_max;
    float steering_angle_target; /* Commanded steering angle as specified by higher level controller */
    float steering_angle_last; /* Last steering angle recorded */
} kia_soul_steering_state_s;


/**
 * @brief Current steering control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    bool operator_override; /* Flag indicating whether steering wheel was manually turned by operator */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_steering_control_state_s;


#endif
