/**
 * @file oscc_signal_smoothing.cpp
 *
 */


 #include "oscc_signal_smoothing.h"


float exponential_moving_average(
    const float alpha,
    const float current_value,
    const float previous_value )
{
    return ( (alpha * current_value) + ((1.0 - alpha) * previous_value) );
}
