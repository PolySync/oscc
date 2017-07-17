/**
 * @file brake_control.h
 * @brief Control of the brake system.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_CONTROL_H_
#define _OSCC_KIA_SOUL_BRAKE_CONTROL_H_


#include <stdint.h>


/*
 * @brief Minimum value of an unsigned 16-bit integer.
 *
 */
#define UINT16_MIN ( 0 )

/*
 * @brief Proportional gain of the PID controller.
 *
 */
#define PID_PROPORTIONAL_GAIN ( 0.65 )

/*
 * @brief Integral gain of the PID controller.
 *
 */
#define PID_INTEGRAL_GAIN ( 1.75 )

/*
 * @brief Derivative gain of the PID controller.
 *
 */
#define PID_DERIVATIVE_GAIN ( 0.001 )

/*
 * @brief Windup guard of the PID controller.
 *
 */
#define PID_WINDUP_GUARD ( 30 )

/*
 * @brief Minimum output value of PID to be within a valid pressure range.
 *
 */
#define PID_OUTPUT_MIN ( -10.0 )

/*
 * @brief Maximum output value of PID to be within a valid pressure range.
 *
 */
#define PID_OUTPUT_MAX ( 10.0 )


/**
 * @brief Current brake control state.
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

    int16_t brake_pressure_front_left; /* Brake pressure at front left wheel. */

    int16_t brake_pressure_front_right; /* Brake pressure at front right wheel. */
} kia_soul_brake_control_state_s;


// ****************************************************************************
// Function:    set_accumulator_solenoid_duty_cycle
//
// Purpose:     Set the PWM that controls the "accumulator" solenoids to the
//              the specified value.
//
// Returns:     void
//
// Parameters:  [in] duty_cycle - value to send to the PWM
//
// ****************************************************************************
void set_accumulator_solenoid_duty_cycle(
    const uint16_t duty_cycle );


// ****************************************************************************
// Function:    set_release_solenoid_duty_cycle
//
// Purpose:     Set the PWM that controls the "release" solenoids to the
//              the specified value.
//
// Returns:     void
//
// Parameters:  [in] duty_cycle - value to send to the PWM
//
// ****************************************************************************
void set_release_solenoid_duty_cycle(
    const uint16_t duty_cycle );


// ****************************************************************************
// Function:    enable_control
//
// Purpose:     Enable control of the brake system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void enable_control( void );


// ****************************************************************************
// Function:    disable_control
//
// Purpose:     Disable control of the brake system.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void disable_control( void );


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
// Function:    check_for_sensor_faults
//
// Purpose:     Checks to see if valid values are being read from the sensors.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_sensor_faults( void );



// ****************************************************************************
// Function:    read_pressure_sensor
//
// Purpose:     Update brake pressure.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void read_pressure_sensor( void );


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
// Function:    update_brake
//
// Purpose:     Write brake spoof values to DAC.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void update_brake( void );


#endif /* _OSCC_KIA_SOUL_BRAKE_CONTROL_H_ */
