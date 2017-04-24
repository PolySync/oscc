/**
* @file gateway_module_state.h
* @brief Gateway module state.
*
**/




#ifndef GATEWAY_MODULE_STATE_H
#define GATEWAY_MODULE_STATE_H




#include <stdint.h>




/**
 * @brief Gateway module state information.
 *
 * Contains state information for the gateway module.
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
} gateway_module_state_s;




int analyze_gateway_state(
        gateway_module_state_s * const state,
        const can_frame_s * const heartbeat_msg_frame,
        const can_frame_s * const chassis_state1_frame,
        const can_frame_s * const chassis_state2_frame );




#endif /* GATEWAY_MODULE_STATE_H */
