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

#ifndef _OSCC_KIA_SOUL_STEERING_MODULE_H_
#define _OSCC_KIA_SOUL_STEERING_MODULE_H_


#include <stdint.h>


/**
 * @brief Override flags.
 *
 * Tracks override flags for wheel and voltages.
 *
 */
typedef struct
{
    bool wheel_active; /* Tracks whether wheel is active */
    uint16_t voltage; /* Tracks any DAC/ADC voltage discrepancies */
    uint16_t voltage_spike_a; /* Used to filter any DAC/ADC voltage spikes */
    uint16_t voltage_spike_b; /* Used to filter any DAC/ADC voltage spikes */
} kia_soul_steering_override_flags_s;


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
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_control_state_s;


#endif
