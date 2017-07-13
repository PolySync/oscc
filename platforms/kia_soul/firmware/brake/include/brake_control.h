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
 * @brief Value of brake pressure that indicates operator override. [decibars]
 *
 */
#define DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ( 43.2 )

/*
 * @brief Brake pressure threshold for when to enable the brake light.
 *
 */
#define BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS (20.0)

/*
 * @brief Minimum possible pressure of brake system. [decibars]
 *
 */
#define BRAKE_PRESSURE_MIN_IN_DECIBARS ( 12.0 )

/*
 * @brief Maximum possible pressure of brake system. [decibars]
 *
 */
#define BRAKE_PRESSURE_MAX_IN_DECIBARS ( 878.3 )

/*
 * @brief Alpha term for the exponential filter used to smooth the sensor input.
 *
 */
#define BRAKE_PRESSURE_SENSOR_EXPONENTIAL_FILTER_ALPHA ( 0.05 )

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

/*
 * @brief Minimum clamped PID value of the actuation solenoid.
 *
 */
#define PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN ( 10.0 )

/*
 * @brief Maximum clamped PID value of the actuation solenoid.
 *
 */
#define PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX ( 110.0 )

/*
 * @brief Minimum clamped PID value of the release solenoid.
 *
 */
#define PID_RELEASE_SOLENOID_CLAMPED_MIN ( 0.0 )

/*
 * @brief Maximum clamped PID value of the release solenoid.
 *
 */
#define PID_RELEASE_SOLENOID_CLAMPED_MAX ( 60.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the actuation solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN ( 80.0 )

/*
 * @brief Maximum duty cycle where actuation solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX ( 105.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the release solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define RELEASE_SOLENOID_DUTY_CYCLE_MIN ( 65.0 )

/*
 * @brief Maximum duty cycle where release solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define RELEASE_SOLENOID_DUTY_CYCLE_MAX ( 100.0 )


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

    bool obd_timeout; /* Flag indicating whether an OBD timeout has occurred. */

    float current_sensor_brake_pressure; /* Current brake pressure as read
                                               from the brake pressure sensor. */

    float current_vehicle_brake_pressure; /* Current brake pressure as reported
                                               by the vehicle. */

    float commanded_pedal_position; /* Brake pedal position commanded by
                                          controller. */
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
