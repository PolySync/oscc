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

#include "PID.h"




void pid_zeroize( PID* pid, double integral_windup_guard )
{
    // set prev and integrated error to zero
    pid->prev_input = 0;
    pid->int_error = 0;
    pid->prev_steering_angle = 0;
    pid->windup_guard = integral_windup_guard;
}


int pid_update( PID* pid, double setpoint, double input, double dt )
{
    double diff;
    double p_term;
    double i_term;
    double d_term;

    double curr_error = setpoint - input;

    static int count = 0;

    if( dt <= 0 )
    {
        return PID_ERROR;
    }

    // integration with windup guarding
    pid->int_error += (curr_error * dt);
    
    count++;
    
    if (pid->int_error < -(pid->windup_guard))
    {
        pid->int_error = -(pid->windup_guard);
    }
    else if (pid->int_error > pid->windup_guard)
    {
        pid->int_error = pid->windup_guard;
    }

    // differentiation
    diff = ((input - pid->prev_input) / dt);
 
    // scaling
    p_term = (pid->proportional_gain * curr_error);
    i_term = (pid->integral_gain     * pid->int_error);
    d_term = (pid->derivative_gain   * diff);
    
    // summation of terms
    pid->control = p_term + i_term - d_term;
 
    // save current error as previous error for next iteration
    pid->prev_input = input;

    return PID_SUCCESS;
}

