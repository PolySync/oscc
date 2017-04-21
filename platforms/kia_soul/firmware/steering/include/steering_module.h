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

#include <Arduino.h>
#include <stdint.h>


/**
 * @brief Override flags.
 *
 * Tracks override flags for wheel and voltages.
 *
 */
typedef struct
{
    uint16_t wheel; /* Tracks whether wheel is active */
    uint16_t voltage; /* Tracks any DAC/ADC voltage discrepancies */
    uint16_t voltage_spike_a; /* Used to filter any DAC/ADC voltage spikes */
    uint16_t voltage_spike_b; /* Used to filter any DAC/ADC voltage spikes */
} kia_soul_steering_override_flags_s;


/*******************************************************************************
*   WARNING
*
*   The ranges selected to do steering control are carefully tested to
*   ensure that a torque is not requested that the vehicles steering motor
*   cannot handle. By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/
typedef struct
{
    float steering_angle_rate_max = 1000.0; /* Maximum rate of change of steering wheel angle */
    float steering_wheel_cutoff_threshold = 3000;
    float SA_Kp = 0.3; /* Proportional gain for PID controller */
    float SA_Ki = 1.3; /* Integral gain for PID controller */
    float SA_Kd = 0.03; /* Derivative gain for PID controller */
    uint32_t windup_guard = 1500; /* Windup guard of the PID controller */
    uint8_t rx_timeout = 250; /* Amount of time when system is considered unresponsive (milliseconds) */
} kia_soul_steering_params_s;


typedef struct
{
    uint8_t dac_cs = 9; /* DAC chip select */
    uint8_t can_cs = 10; /* CAN chip select */
    uint8_t signal_torque_pos_sensor_high = A0; /* High signal from torque position sensor */
    uint8_t signal_torque_pos_sensor_low = A1; /* Low signal from torque position sensor */
    uint8_t spoof_signal_high = A2; /* High signal of spoof output */
    uint8_t spoof_signal_low = A3; /* Low signal of spoof output */
    uint8_t spoof_enable = 6; /* Relay enable for spoofed torque values */
} kia_soul_steering_pins_s;


/**
 * @brief Current steering state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    float steering_angle; /* Current steering angle as reported by car */
    float steering_angle_target; /* Commanded steering angle as specified by higher level controller */
    float steering_angle_last; /* Last steering angle recorded */
} kia_soul_steering_state_s;


typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_control_state_s;


typedef struct
{
    kia_soul_steering_state_s state; /* State of the steering system */
    kia_soul_control_state_s control_state; /* Control state of the steering system */
    kia_soul_steering_params_s params; /* Parameters of the steering system */
    kia_soul_steering_pins_s pins; /* Pin assignments of the sensor interface board */
    kia_soul_steering_override_flags_s override_flags; /* Override flags */
} kia_soul_steering_module_s;

#endif
