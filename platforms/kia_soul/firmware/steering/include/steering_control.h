/**
 * @file steering_control.h
 * @brief Control of the steering system.
 *
 */


#ifndef _OSCC_KIA_SOUL_STEERING_CONTROL_H_
#define _OSCC_KIA_SOUL_STEERING_CONTROL_H_


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

    float commanded_steering_angle; /* Angle of steering wheel commanded by
                                       controller. */

    float steering_angle; /* Current steering angle as reported by the vehicle. */

    float steering_angle_last; /* Last steering angle recorded. */
} kia_soul_steering_control_state_s;


// ****************************************************************************
// Function:    check_for_operator_override
//
// Purpose:     Checks to see if the vehicle's operator has manually pressed
//              the accelerator and disables control if they have.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_operator_override( void );


// ****************************************************************************
// Function:    update_steering
//
// Purpose:     Writes steering spoof values to DAC.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void update_steering( void );


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


#endif /* _OSCC_KIA_SOUL_STEERING_CONTROL_H_ */
