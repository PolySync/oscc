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
 * @brief Minimum possible value expected to be read from the brake pressure
 * sensors when the pressure check pins (PCK1/PCK2) are asserted.
 *
 */
#define BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN ( 665 )

/*
 * @brief Maximum possible value expected to be read from the brake pressure
 * sensors when the pressure check pins (PCK1/PCK2) are asserted.
 *
 */
#define BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MAX ( 680 )

/*
 * @brief Amount of time between sensor checks. [milliseconds]
 *
 */
#define SENSOR_VALIDITY_CHECK_INTERVAL_IN_MSEC ( 250 )

/*
 * @brief Number of consecutive faults that can occur when reading the
 *        sensors before control is disabled.
 *
 */
#define SENSOR_VALIDITY_CHECK_FAULT_COUNT ( 4 )

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

<<<<<<< HEAD:firmware/kia_soul/brake/include/brake_control.h
    uint8_t dtcs; /* Bitfield of faults present in the module. */
=======
    bool startup_pressure_check_error; /* Flag indicating a problem with the actuator. */

    bool startup_pump_motor_check_error; /* Flag indicating a problem with the pump motor. */

    float current_sensor_brake_pressure; /* Current brake pressure as read
                                               from the brake pressure sensor. */
>>>>>>> devel:platforms/kia_soul/firmware/brake/include/brake_control.h

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
