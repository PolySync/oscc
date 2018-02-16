/**
 * @file pid.h
 * @brief PID utilities.
 *
 */


#ifndef _OSCC_PID_H_
#define _OSCC_PID_H_


/**
 * @brief Math macro: constrain(amount, low, high).
 *
 */
#define m_constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

/**
 * @brief Error in PID calculation.
 *
 */
#define PID_ERROR 1

/**
 * @brief Success in PID calculation.
 *
 */
#define PID_SUCCESS 0


/*
 * @brief PID components.
 *
 */
typedef struct
{
    float windup_guard; /* Windup guard. */

    float proportional_gain; /* Proportional gain. */

    float integral_gain; /* Integral gain. */

    float derivative_gain; /* Derivative gain. */

    float prev_input; /* Previous input. */

    float int_error; /* Error. */

    float control; /* Control. */

    float prev_steering_angle; /* Previous steering angle. */
} pid_s;


// ****************************************************************************
// Function:    pid_update
//
// Purpose:     Update the values in the PID structure.
//
// Returns:     int - \ref PID_SUCCESS or \ref PID_ERROR
//
// Parameters:  [out] pid - structure containing existing PID data that will
//                             be updated
//              [in] setpoint - goal value to obtain
//              [in] input - current value
//              [in] dt - differentiation value
//
// ****************************************************************************
int pid_update( pid_s* pid, float setpoint, float input, float dt );


// ****************************************************************************
// Function:    pid_zeroize
//
// Purpose:     Update the values in the PID structure.
//
// Returns:     int - \ref PID_SUCCESS or \ref PID_ERROR
//
// Parameters:  [out] pid - PID stucture to fill with zeros
//              [in] integral_windup_guard - windup guard value to set
//
// ****************************************************************************
void pid_zeroize( pid_s* pid, float integral_windup_guard );


#endif /* _OSCC_PID_H_ */
