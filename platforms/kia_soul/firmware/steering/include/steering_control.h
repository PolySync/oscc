#ifndef _OSCC_KIA_SOUL_STEERING_CONTROL_H_
#define _OSCC_KIA_SOUL_STEERING_CONTROL_H_


#include <stdint.h>


/**
 * @brief Torque spoof values.
 *
 * Contains the high and low torque spoof values.
 *
 */
typedef struct
{
    uint16_t low;
    uint16_t high;
} steering_torque_s;


/**
 * @brief Current steering control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool operator_override; /* Flag indicating whether steering wheel was manually turned by operator */
    float commanded_steering_angle; /* Angle of steering wheel commanded by controller */
    float steering_angle; /* Current steering angle as reported by car */
    float steering_angle_last; /* Last steering angle recorded */
} kia_soul_steering_control_state_s;


// *****************************************************
// Function:    check_for_operator_override
//
// Purpose:     This function checks the voltage input from the steering
//              wheel's torque sensors to determine if the driver is attempting
//              to steer the vehicle.  This must be done over time by taking
//              periodic samples of the input torque voltage, calculating the
//              difference between the two and then passing that difference
//              through a basic exponential filter to smooth the input.
//
//              The required response time for the filter is 250 ms, which at
//              50ms per sample is 5 samples.  As such, the alpha for the
//              exponential filter is 0.5 to make the input go "close to" zero
//              in 5 samples.
//
//              The implementation is:
//                  s(t) = ( a * x(t) ) + ( ( 1 - a ) * s ( t - 1 ) )
//
//              If the filtered torque exceeds the max torque, it is an
//              indicator that there is feedback on the steering wheel and the
//              control should be disabled.
//
//              The final check determines if the a and b signals are opposite
//              each other.  If they are not, it is an indicator that there is
//              a problem with one of the sensors.  The check is looking for a
//              90% tolerance.
//
// Returns:     true if the driver is requesting an override
//
// Parameters:  void
//
// *****************************************************
void check_for_operator_override( void );


// *****************************************************
// Function:    update_steering
//
// Purpose:     Writes steering spoof values to DAC.
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void update_steering( void );


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
