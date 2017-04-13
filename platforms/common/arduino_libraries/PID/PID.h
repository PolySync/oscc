/************************************************************************/
/* The MIT License (MIT)                                                */
/* =====================                                                */
/*                                                                      */
/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the “Software”), to deal in the Software without              */
/* restriction, including without limitation the rights to use,         */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell    */
/* copies of the Software, and to permit persons to whom the            */
/* Software is furnished to do so, subject to the following             */
/* conditions:                                                          */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        */
/* OTHER DEALINGS IN THE SOFTWARE.                                      */
/************************************************************************/

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
    double windup_guard;
    double proportional_gain;
    double integral_gain;
    double derivative_gain;
    double prev_input;
    double int_error;
    double control;
    double prev_steering_angle;
} PID;




int pid_update( PID* pid, double setpoint, double input, double dt );


void pid_zeroize( PID* pid, double integral_windup_guard );




#ifdef __cplusplus
}
#endif
 



#endif /* PID_H */


