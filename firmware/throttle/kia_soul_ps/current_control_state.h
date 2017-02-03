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
     uint16_t pedal; /* Tracks whether pedal is pressed */
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
    double pedal_position; /* Current pedal position as reported by car */
    //
    //
    double pedal_position_target; /* As specified by higher level controller */
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
