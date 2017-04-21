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

#ifndef _OSCC_KIA_SOUL_THROTTLE_STATE_H_
#define _OSCC_KIA_SOUL_THROTTLE_STATE_H_

#include <Arduino.h>
#include <stdint.h>


/**
 * @brief Override flags.
 *
 * Tracks override flags for accelerator and voltages.
 *
 */
typedef struct
{
    uint16_t accelerator; /* Tracks whether accelerator is pressed */
    uint16_t voltage; /* Tracks any DAC/ADC voltage discrepancies */
    uint16_t voltage_spike_a; /* Used to filter any DAC/ADC voltage spikes */
    uint16_t voltage_spike_b; /* Used to filter any DAC/ADC voltage spikes */
} kia_soul_throttle_override_flags_s;


typedef struct
{
    float accelerator_threshold = 1000.0; /* Threshhold to detect when a person is pressing accelerator */
    uint8_t rx_timeout = 250; /* Amount of time when system is considered unresponsive (milliseconds) */
} kia_soul_throttle_params_s;


typedef struct
{
    uint8_t dac_cs = 9; /* DAC chip select */
    uint8_t can_cs = 10; /* CAN chip select */
    uint8_t signal_accel_pos_sensor_high = A0; /* High signal from accelerator position sensor */
    uint8_t signal_accel_pos_sensor_low = A1; /* Low signal from accelerator position sensor */
    uint8_t spoof_signal_high = A2; /* High signal of spoof output */
    uint8_t spoof_signal_low = A3; /* Low signal of spoof output */
    uint8_t spoof_enable = 6; /* Relay enable for spoofed accel values */
} kia_soul_throttle_pins_s;

/**
 * @brief Current throttle state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */
typedef struct
{
    //
    //
    uint16_t accel_pos_sensor_high; /* Value of high signal of accelerator position sensor */
    //
    //
    uint16_t accel_pos_sensor_low; /* Value of low signal of accelerator position sensor */
    //
    //
    float accel_pos_target; /* As specified by higher level controller */
} kia_soul_throttle_state_s;


typedef struct
{
    bool enabled; /* Flag indicating control is currently enabled */
    bool emergency_stop; /* Flag indicating emergency stop has been acitivated */
    uint32_t timestamp_us; /* Keeps track of last control loop time in us */
} kia_soul_throttle_control_state_s;


typedef struct
{
    //
    //
    kia_soul_throttle_state_s state; /* State of the throttle system */
    //
    //
    kia_soul_throttle_control_state_s control_state; /* Control state of the throttle system */
    //
    //
    kia_soul_throttle_params_s params; /* Parameters of the throttle system */
    //
    //
    kia_soul_throttle_pins_s pins; /* Pin assignments of the sensor interface board */
    //
    //
    kia_soul_throttle_override_flags_s override_flags; /* Override flags */
} kia_soul_throttle_module_s;

#endif
