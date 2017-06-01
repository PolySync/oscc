/**
* @file steering_module_state.h
* @brief Steering module state.
*
**/




#ifndef STEERING_MODULE_STATE_H
#define STEERING_MODULE_STATE_H




#include <stdint.h>

#include "can_monitor.h"




/**
 * @brief Steering module state information.
 *
 * Contains state information for the steering module.
 *
 */
typedef struct
{
    //
    //
    uint8_t module_state;
    //
    //
    uint8_t control_state;
    //
    //
    uint8_t override_triggered;
} steering_module_state_s;




int analyze_steering_state(
        steering_module_state_s * const state,
        const can_frame_s * const steering_command_frame,
        const can_frame_s * const steering_report_frame );




#endif /* STEERING_MODULE_STATE_H */
