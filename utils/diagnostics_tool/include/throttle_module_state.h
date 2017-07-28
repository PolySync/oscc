/**
* @file throttle_module_state.h
* @brief Throttle module state.
*
**/




#ifndef THROTTLE_MODULE_STATE_H
#define THROTTLE_MODULE_STATE_H




#include <stdint.h>




/**
 * @brief Throttle module state information.
 *
 * Contains state information for the throttle module.
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
} throttle_module_state_s;




int analyze_throttle_state(
        throttle_module_state_s * const state,
        const can_frame_s * const throttle_command_frame,
        const can_frame_s * const throttle_report_frame );




#endif /* THROTTLE_MODULE_STATE_H */
