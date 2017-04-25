#ifndef _OSCC_KIA_SOUL_STEERING_CONTROL_H_
#define _OSCC_KIA_SOUL_STEERING_CONTROL_H_


#include <stdint.h>
#include "DAC_MCP49xx.h"


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
} torque_spoof_t;


/**
 * @brief Current steering state.
 *
 * Keeps track of the current state of the steering system.
 *
 */
typedef struct
{
    float steering_angle; /* Current steering angle as reported by car */
    float steering_angle_rate_max;
    float steering_angle_target; /* Commanded steering angle as specified by higher level controller */
    float steering_angle_last; /* Last steering angle recorded */
} kia_soul_steering_state_s;


/**
 * @brief Current steering control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    bool operator_override; /* Flag indicating whether steering wheel was manually turned by operator */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_steering_control_state_s;


// *****************************************************
// Function:    check_driver_steering_override
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
bool check_driver_steering_override( void );


// *****************************************************
// Function:    calculate_torque_spoof
//
// Purpose:     Container for hand-tuned empirically determined values
//
//              Values calculated with min/max calibration curve and hand
//              tuned for neutral balance.
//              DAC requires 12-bit values = (4096steps/5V = 819.2 steps/V)
//
// Returns:     void
//
// Parameters:  [in] torque - floating point value with the current torque value
//              [out] torque_spoof - structure containing the integer torque values
//
// *****************************************************
void calculate_torque_spoof(
    const float torque,
    torque_spoof_t * const spoof );

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
