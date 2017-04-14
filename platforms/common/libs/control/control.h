#ifndef _OSCC_CONTROL_H_
#define _OSCC_CONTROL_H_

#include <stdint.h>

#include "DAC_MCP49xx.h"


typedef struct
{
    //
    //
    bool enabled; /* Is control currently enabled flag */
    //
    //
    bool emergency_stop; /* Emergency stop has been acitivated by higher level controller */
    //
    //
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} control_state_s;


// *****************************************************
// Function:    enable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  signal_pin_1 - pin number of first signal to average
//              signal_pin_2 - pin number of second signal to average
//              relay_pin - pin number of relay to enable to activate control
//              state - pointer to struct containing state information
//              dac - pointer to DAC struct
//
// *****************************************************
void enable_control(
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        uint8_t relay_pin,
        control_state_s *state,
        DAC_MCP49xx *dac );


// *****************************************************
// Function:    disable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  signal_pin_1 - pin number of first signal to average
//              signal_pin_2 - pin number of second signal to average
//              relay_pin - pin number of relay to enable to activate control
//              state - pointer to struct containing state information
//              dac - pointer to DAC struct
//
// *****************************************************
void disable_control(
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        uint8_t relay_pin,
        control_state_s *state,
        DAC_MCP49xx *dac );


#endif
