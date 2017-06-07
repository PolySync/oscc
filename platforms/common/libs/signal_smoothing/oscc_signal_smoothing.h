/**
 * @file oscc_signal_smoothing.h
 * @brief Signal smoothing utilities.
 *
 */


#ifndef _OSCC_SIGNAL_SMOOTHING_H_
#define _OSCC_SIGNAL_SMOOTHING_H_


// ****************************************************************************
// Function:    exponential_moving_average
//
// Purpose:     Calculates an exponential moving average.
//
//              s(t) = (a * x(t)) + ((1-a) * s(t-1))
//
//              s(t) - smoothed signal
//              a - alpha
//              x(t) - current noisy value of signal
//              s(t-1) - previous output of the function
//
// Returns:     float - smoothed value
//
// Parameters:  alpha - alpha term of the exponential moving average
//              current_value - current value to be fed into the function
//                              for smoothing
//              previous_value - previous value that was returned from the
//                               function after smoothing
//
// ****************************************************************************
float exponential_moving_average(
    const float alpha,
    const float current_value,
    const float previous_value);


#endif /* _OSCC_SIGNAL_SMOOTHING_H_ */
