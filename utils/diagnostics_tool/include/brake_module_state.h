/**
* @file brake_module_state.h
* @brief Brake module state.
*
**/




#ifndef BRAKE_MODULE_STATE_H
#define BRAKE_MODULE_STATE_H




#include <stdint.h>




/**
 * @brief Brake module state information.
 *
 * Contains state information for the brake module.
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
} brake_module_state_s;




int analyze_brake_state(
        brake_module_state_s * const state,
        const can_frame_s * const brake_command_frame,
        const can_frame_s * const brake_report_frame );




#endif /* BRAKE_MODULE_STATE_H */
