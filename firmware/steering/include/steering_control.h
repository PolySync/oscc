/**
 * @file steering_control.h
 * @brief Control of the steering system.
 *
 */


#ifndef _OSCC_STEERING_CONTROL_H_
#define _OSCC_STEERING_CONTROL_H_


#include <stdint.h>


/**
 * @brief Torque values.
 *
 * Contains the high and low torque values.
 *
 */
typedef struct
{
    uint16_t low; /* Low value of torque. */

    uint16_t high; /* High value of torque. */
} steering_torque_s;


/**
 * @brief Current steering control state.
 *
 * Current state of the throttle module control system.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled. */

    bool operator_override; /* Flag indicating whether steering wheel was
                               manually turned by operator. */

    uint8_t dtcs; /* Bitfield of faults present in the module. */
} steering_control_state_s;


// ****************************************************************************
// Function:    check_for_sensor_faults
//
// Purpose:     Checks to see if valid values are being read from the sensors.
//              If operator override for steering is turned on detection of the
//              steering wheel being manually turned is also detected.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_faults( void );


// ****************************************************************************
// Function:    update_steering
//
// Purpose:     Writes steering spoof values to DAC.
//
// Returns:     void
//
// Parameters:  spoof_command_high - high value of spoof command
//              spoof_command_low - low value of spoof command
//
// ****************************************************************************
void update_steering(
    uint16_t spoof_command_high,
    uint16_t spoof_command_low );


// *****************************************************
// Function:    enable_control
//
// Purpose:     Enable control of the steering system.
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
// Purpose:     Disable control of the steering system.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void disable_control( void );


#endif /* _OSCC_STEERING_CONTROL_H_ */
