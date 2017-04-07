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

/**
 * @brief Current control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */

 typedef struct /* Tracks override flags for pedal and voltages */
 {
     uint16_t wheel; /* Tracks whether pedal is pressed */
     uint16_t voltage; /* Tracks any DAC/ADC voltage discrepancies */
     uint16_t voltage_spike_a; /* Used to filter any DAC/ADC voltage spikes */
     uint16_t voltage_spike_b; /* Used to filter any DAC/ADC voltage spikes */
 } override_flags;

typedef struct
{
    //
    //
    bool control_enabled; /* Is control currently enabled flag */
    //
    //
    bool emergency_stop; /* Emergency stop has been acitivated by higher level controller */
    //
    //
    double current_steering_angle; /* Current steering angle as reported by car */
    //
    //
    double commanded_steering_angle; /* Commanded steering angle as specified by higher level controller */
    //
    //
    double PID_input; /* Input to PID controller */
    //
    //
    double PID_output; /* Output from PID controller */
    //
    //
    double PID_setpoint; /* Setpoint for PID controller */
    //
    //
/*******************************************************************************
*   WARNING
*
*   The PID gains (SA_Kp, SA_Ki, SA_Kd) are carefully tested to ensure that a
*   torque is not requested that the vehicles steering motor cannot handle.
*   By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/
    double SA_Kp = 0.3; /* Proportional gain for PID controller */
    //
    //
    double SA_Ki = 1.3; /* Integral gain for PID controller */
    //
    //
    double SA_Kd = 0.03; /* Derivative gain for PID controller */
    //
    //
    double steering_angle_rate_max = 1000.0; /* Maximum rate of change of steering wheel angle */
    //
    //
    double steering_angle_last; /* Last steering angle recorded */
    //
    //
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
    //
    //
    override_flags override_flag;
    //
    //
    uint16_t test_countdown; /* Iterator for DAC/ADC Voltage check */
    //
    //
} current_control_state;
