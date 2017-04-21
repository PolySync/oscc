#ifndef _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_
#define _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_


struct accel_spoof_t
{
    uint16_t low;
    uint16_t high;
};


void calculate_accelerator_spoof(
    float accelerator_target,
    struct accel_spoof_t* spoof );


void check_accelerator_override( );


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
void enable_control( );


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
void disable_control( );


#endif
