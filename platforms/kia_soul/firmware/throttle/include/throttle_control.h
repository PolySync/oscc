#ifndef _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_
#define _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_

#include "DAC_MCP49xx.h"

struct torque_spoof_t
{
    uint16_t low;
    uint16_t high;
};

void calculate_pedal_spoof(
    float pedal_target,
    struct torque_spoof_t* spoof );

void check_pedal_override(
    kia_soul_throttle_module_s *throttle_module,
    DAC_MCP49xx &dac );

// *****************************************************
// Function:    enable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  throttle_module - pointer to struct containing throttle module information
//              dac - reference to DAC object
//
// *****************************************************
void enable_control(
        kia_soul_throttle_module_s *throttle_module,
        DAC_MCP49xx &dac );


// *****************************************************
// Function:    disable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  throttle_module - pointer to struct containing throttle module information
//              dac - reference to DAC object
//
// *****************************************************
void disable_control(
        kia_soul_throttle_module_s *throttle_module,
        DAC_MCP49xx &dac );


#endif
