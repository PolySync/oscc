#ifndef _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_
#define _OSCC_KIA_SOUL_THROTTLE_CONTROL_H_


#include <stdint.h>


/**
 * @brief Accelerator values.
 *
 * Contains high and low accelerator values.
 *
 */
typedef struct
{
    uint16_t low;
    uint16_t high;
} accelerator_position_s;


/**
 * @brief Current throttle control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating whether control is currently enabled */
    bool operator_override; /* Flag indicating whether accelerator was manually pressed by operator */
    float commanded_accelerator_position; /* Position of accelerator commanded by controller */
} kia_soul_throttle_control_state_s;


// *****************************************************
// Function:    check_for_operator_override
//
// Purpose:     This function checks to see if the vehicle's
//              operator has manually pressed the accelerator.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void check_for_operator_override( void );


// *****************************************************
// Function:    read_accelerator_position_sensor
//
// Purpose:     Reads from accelerator position sensor.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void read_accelerator_position_sensor( accelerator_position_s * value );


// *****************************************************
// Function:    update_throttle
//
// Purpose:     Writes throttle spoof values to DAC.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void update_throttle( void );


// *****************************************************
// Function:    enable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void enable_control( void );


// *****************************************************
// Function:    disable_control
//
// Purpose:     Sample the current value being written and smooth it out by
//              averaging it out over several samples, write that value to the
//              DAC, and then enable the control
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void disable_control( void );


#endif
