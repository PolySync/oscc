#ifndef _OSCC_KIA_SOUL_BRAKE_MODULE_H_
#define _OSCC_KIA_SOUL_BRAKE_MODULE_H_


#include "accumulator.h"


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


#endif
