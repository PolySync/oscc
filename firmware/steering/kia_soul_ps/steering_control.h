/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* This file is part of Open Source Car Control (OSCC).                 */
/*                                                                      */
/* OSCC is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* OSCC is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with OSCC.  If not, see <http://www.gnu.org/licenses/>.        */
/************************************************************************/

// Header for steering control ECU firmware
// 2014 Kia Soul Motor Driven Power Steering (MDPS) system

#ifndef STEERING_CONTROL_H
#define STEERING_CONTROL_H

#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "current_control_state.h"
#include "PID.h"
#include "common.h"
#include "DAC_MCP49xx.h"

// set CAN_CS to pin 10 for CAN
#define CAN_CS                          ( 10 )

// ms
#define PS_CTRL_RX_WARN_TIMEOUT         ( 250 )

// set up pins for interface with DAC (MCP4922)
#define DAC_CS                          ( 9 )       // Chip select pin

// Windup guard for steering PID controller
#define STEERING_WINDUP_GUARD           ( 1500 )

// Signal to ADC from car
#define SIGNAL_INPUT_A                  ( A0 )

// Green wire from the torque sensor, low values
#define SIGNAL_INPUT_B                  ( A1 )

// Spoof signal from DAC out to car
#define SPOOF_SIGNAL_A                  ( A2 )

// Blue wire from the torque sensor, high values
#define SPOOF_SIGNAL_B                  ( A3 )

// Signal interrupt (relay) for spoofed torque values
#define SPOOF_ENGAGE                    ( 6 )

// Threshhold to detect when a person is turning the steering wheel
#define STEERING_WHEEL_CUTOFF_THRESHOLD ( 3000 )

// Threshhold to detect when there is a discrepancy between DAC and ADC values
#define VOLTAGE_THRESHOLD               ( 0.096 )     // mV

#define SAMPLE_A                        ( 0 )

#define SAMPLE_B                        ( 1 )

#define FAILURE                         ( 0 )

#define SUCCESS                         ( 1 )

// *****************************************************
// Function:    init_serial
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void init_serial( );

// *****************************************************
// Function:    init_can
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void init_can ( void );

// *****************************************************
// Function:    timer_delta_ms
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] timestamp - the last time sample
//
// *****************************************************
uint32_t timer_delta_ms( uint32_t last_time );

// *****************************************************
// Function:    timer_delta_us
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] last_sample - the last time sample
//              [in] current_sample - pointer to store the current time
//
// *****************************************************
uint32_t timer_delta_us( uint32_t last_time, uint32_t* current_time );

// *****************************************************
// Function:    average_samples
//
// Returns:     int16_t - SUCCESS or FAILURE
//
// Parameters:  [in]  num_samples - the number of samples to average
//              [out] averages - array of values to store the averages
//
// *****************************************************
int16_t average_samples( int16_t num_samples, int16_t* averages );


// *****************************************************
// Function:    enable_control
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void enable_control( );

// *****************************************************
// Function:    disable_control
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void disable_control( );

// *****************************************************
// Function:    do_dac_output
//
// Returns:     void
//
// Parameters:  [in] torque_spoof - the spoofed torque struct
//
// *****************************************************
void do_dac_output( struct torque_spoof_t* torque_spoof );

// *****************************************************
// Function:    check_driver_steering_override
//
// Returns:     true if the driver is requesting an override
//
// Parameters:  None
//
// *****************************************************
bool check_driver_steering_override( );


// *****************************************************
// Function:    calculate_torque_spoof
//
// Returns:     void
//
// Parameters:  [in] torque - floating point value with the current torque value
//              [out] torque_spoof - structure containing the integer torque values
//
// *****************************************************
void calculate_torque_spoof( float torque, struct torque_spoof_t* spoof );

// *****************************************************
// Function:    publish_ps_ctrl_steering_report
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void publish_ps_ctrl_steering_report( );


// *****************************************************
// Function:    publish_timed_tx_frames
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void publish_timed_tx_frames( );

// *****************************************************
// Function:    process_ps_ctrl_steering_command
//
// Returns:     void
//
// Parameters:  [in] control_data -  pointer to a steering command control message
//
// *****************************************************
void process_ps_ctrl_steering_command(
        const ps_ctrl_steering_command_msg * const control_data );

// *****************************************************
// Function:    process_psvc_chassis_state1
//
// Returns:     void
//
// Parameters:  [in] chassis_data - pointer to a chassis state message that contains
//                             		the steering angle
//
// *****************************************************
void process_psvc_chassis_state1(
    const psvc_chassis_state1_data_s * const chassis_data );

// *****************************************************
// Function:    handle_ready_rx_frames
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void handle_ready_rx_frames( );

// *****************************************************
// Function:    check_rx_timeouts
//
// Returns:     void
//
// Parameters:  None
//
// *****************************************************
void check_rx_timeouts( );

#endif /* STEERING_CONTROL_H */