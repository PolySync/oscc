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




#ifdef __cplusplus
extern "C" {
#endif




typedef struct 
{
    double windup_guard;
    double proportional_gain;
    double integral_gain;
    double derivative_gain;
    double prev_error;
    double int_error;
    double control;
    double prev_steering_angle;
} PID;




void pid_update( PID* pid, double curr_error, double dt );


void pid_zeroize( PID* pid );




#ifdef __cplusplus
}
#endif
 



#endif /* PID_H */


