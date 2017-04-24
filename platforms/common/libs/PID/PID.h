/*
 * File:   PID.h
 *
 */

#ifndef PID_H
#define PID_H




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




#ifdef __cplusplus
extern "C" {
#endif




typedef struct
{
    float windup_guard;
    float proportional_gain;
    float integral_gain;
    float derivative_gain;
    float prev_input;
    float int_error;
    float control;
    float prev_steering_angle;
} PID;




int pid_update( PID* pid, float setpoint, float input, float dt );


void pid_zeroize( PID* pid, float integral_windup_guard );




#ifdef __cplusplus
}
#endif




#endif /* PID_H */


