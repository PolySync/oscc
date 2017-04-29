/**
 * @file brake_control.h
 * @brief Control of the brake system.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_CONTROL_H_
#define _OSCC_KIA_SOUL_BRAKE_CONTROL_H_


#include <stdint.h>


/**
 * @brief Current brake control state.
 *
 * Current state of the throttle module control system.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */

    bool operator_override; /* Flag indicating whether brake pedal was
                               manually pressed by operator. */

    uint16_t commanded_pedal_position; /* Brake pedal position commanded by
                                          controller. */

    float accumulator_pressure; /* Pressure in the accumulator. */

    float current_pressure; /* Current brake pressure. */

    float can_pressure; /* Pressure reported by chassis state. */
} kia_soul_brake_control_state_s;


// ****************************************************************************
// Function:    brake_lights_off
//
// Purpose:     Turn the brake lights off.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_lights_off( void );


// ****************************************************************************
// Function:    brake_lights_on
//
// Purpose:     Turn the brake lights on.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_lights_on( void );


// ****************************************************************************
// Function:    brake_command_actuator_solenoids
//
// Purpose:     Set the PWM that controls the "actuator" solenoids to the
//              the specified value.
//
// Returns:     void
//
// Parameters:  [in] duty_cycle - value to send to the PWM
//
// ****************************************************************************
void brake_command_actuator_solenoids(
    const uint16_t duty_cycle );


// ****************************************************************************
// Function:    brake_command_release_solenoids
//
// Purpose:     Set the PWM that controls the "release" solenoids to the
//              the specified value.
//
// Returns:     void
//
// Parameters:  [in] duty_cycle - value to send to the PWM
//
// ****************************************************************************
void brake_command_release_solenoids(
    const uint16_t duty_cycle );


// ****************************************************************************
// Function:    brake_enable
//
// Purpose:     Enable control of the brake system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_enable( void );


// ****************************************************************************
// Function:    brake_disable
//
// Purpose:     Disable control of the brake system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_disable( void );


// ****************************************************************************
// Function:    check_for_operator_override
//
// Purpose:     Check to see if the vehicle's operator has manually pressed
//              the brake pedal and disable control if they have.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_operator_override( void );


// ****************************************************************************
// Function:    brake_update_pressure
//
// Purpose:     Update brake pressure.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_update_pressure( void );


// ****************************************************************************
// Function:    brake_init
//
// Purpose:     Initialize the brake system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_init( void );


// ****************************************************************************
// Function:    brake_update
//
// Purpose:     Write brake spoof values to DAC.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void brake_update( void );


#endif /* _OSCC_KIA_SOUL_BRAKE_CONTROL_H_ */
