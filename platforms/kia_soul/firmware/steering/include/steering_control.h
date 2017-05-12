/**
 * @file steering_control.h
 * @brief Control of the steering system.
 *
 */


#ifndef _OSCC_KIA_SOUL_STEERING_CONTROL_H_
#define _OSCC_KIA_SOUL_STEERING_CONTROL_H_


#include <stdint.h>


/*******************************************************************************
*   WARNING
*
*   The ranges selected to do steering control are carefully tested to
*   ensure that a torque is not requested that the vehicles steering motor
*   cannot handle. By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/

/*
 * @brief Minimum steering angle rate. [Newton meters]
 *
 */
#define TORQUE_MIN_IN_NEWTON_METERS ( -1500.0 )

/*
 * @brief Maximum steering angle rate. [Newton meters]
 *
 */
#define TORQUE_MAX_IN_NEWTON_METERS ( 1500.0 )

/*
 * @brief Proportional gain of the PID controller.
 *
 */
#define PID_PROPORTIONAL_GAIN ( 0.3 )

/*
 * @brief Integral gain of the PID controller.
 *
 */
#define PID_INTEGRAL_GAIN ( 1.3 )

/*
 * @brief Derivative gain of the PID controller.
 *
 */
#define PID_DERIVATIVE_GAIN ( 0.03 )

/*
 * @brief Value of the torque sensor that indicates operator override.
          [degrees/microsecond]
 *
 */
#define OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC ( 3000 )

/*
 * @brief Number of bits to shift to go from a 10-bit value to a 12-bit value.
 *
 */
#define BIT_SHIFT_10BIT_TO_12BIT ( 2 )

/*
 * @brief Number of steps per volt corresponding to 4096 steps across 5 volts.
 *
 */
#define STEPS_PER_VOLT ( 819.2 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALAR ( 0.0008 )

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.26 )

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALAR ( -0.0008 )

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.5 )


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

    int16_t current_steering_wheel_angle; /* Current steering angle as reported
                                             by the vehicle. */

    int16_t previous_steering_wheel_angle; /* Last steering angle recorded. */

    int16_t commanded_steering_wheel_angle; /* Angle of steering wheel commanded
                                               by controller. */

    float commanded_steering_wheel_angle_rate; /* Rate of the steering wheel
                                                  angle commanded by controller. */
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
