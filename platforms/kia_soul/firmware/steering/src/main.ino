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

// Steering control ECU firmware
// 2014 Kia Soul Motor Driven Power Steering (MDPS) system


#include <SPI.h>
#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "steering_protocol_can.h"
#include "PID.h"
#include "DAC_MCP49xx.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "steering_module.h"
#include "init.h"
#include "steering_control.h"
#include "communications.h"


static kia_soul_steering_module_s steering_module;
static DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, steering_module.pins.dac_cs );     // DAC model, SS pin, LDAC pin
static MCP_CAN CAN( steering_module.pins.can_cs );                          // Set CS pin for the CAN shield
static can_frame_s rx_frame_steering_command;
static can_frame_s tx_frame_steering_report;
static PID pid;
static uint8_t torque_sum;


void setup( )
{
    memset( &rx_frame_steering_command,
            0,
            sizeof(rx_frame_steering_command) );

    init_pins( &steering_module );

    #ifdef DEBUG
        init_serial( );
    #endif

    init_can( CAN );

    publish_timed_tx_frames(
        &steering_module,
        &tx_frame_steering_report,
        CAN,
        torque_sum );

    steering_module.control_state.enabled = false;

    steering_module.control_state.emergency_stop = false;

    steering_module.override_flags.wheel = 0;

    steering_module.override_flags.voltage = 0;

    steering_module.override_flags.voltage_spike_a = 0;

    steering_module.override_flags.voltage_spike_b = 0;

    // Initialize the Rx timestamps to avoid timeout warnings on start up
    rx_frame_steering_command.timestamp = millis( );

    pid_zeroize( &pid, steering_module.params.windup_guard );

    // debug log
    DEBUG_PRINTLN( "init: pass" );
}


void loop( )
{
    // checks for CAN frames, if yes, updates state variables
    can_frame_s rx_frame;
    int ret = check_for_rx_frame( CAN, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
        handle_ready_rx_frames(
            &steering_module,
            &rx_frame,
            &rx_frame_steering_command,
            dac );
    }

    // publish all report CAN frames
    publish_timed_tx_frames(
        &steering_module,
        &tx_frame_steering_report,
        CAN,
        torque_sum );

    // check all timeouts
    check_rx_timeouts(
        &steering_module,
        &rx_frame_steering_command,
        dac );

    uint32_t current_timestamp_us = GET_TIMESTAMP_US();

    uint32_t deltaT = get_time_delta( steering_module.control_state.timestamp_us,
                                      current_timestamp_us );

    if ( deltaT > 50000 )
    {

        steering_module.control_state.timestamp_us = current_timestamp_us;

        bool override = check_driver_steering_override( &steering_module );

        if ( override == true
             && steering_module.control_state.enabled == true )
        {
            steering_module.override_flags.wheel = 1;
            disable_control( &steering_module, dac );
        }
        else if ( steering_module.control_state.enabled == true )
        {
            // Calculate steering angle rates (degrees/microsecond)
            double steering_angle_rate =
                ( steering_module.state.steering_angle -
                  steering_module.state.steering_angle_last ) / 0.05;

            double steering_angle_rate_target =
                ( steering_module.state.steering_angle_target -
                  steering_module.state.steering_angle ) / 0.05;

            // Save the angle for next iteration
            steering_module.state.steering_angle_last =
                steering_module.state.steering_angle;

            steering_angle_rate_target =
                constrain( ( double )steering_angle_rate_target,
                           ( double )-steering_module.params.steering_angle_rate_max,
                           ( double )steering_module.params.steering_angle_rate_max );

            pid.derivative_gain = steering_module.params.SA_Kd;
            pid.proportional_gain = steering_module.params.SA_Kp;
            pid.integral_gain = steering_module.params.SA_Ki;

            pid_update(
                    &pid,
                    steering_angle_rate_target,
                    steering_angle_rate,
                    0.050 );

            double control = pid.control;

            control = constrain( ( float ) control,
                                 ( float ) -1500.0f,
                                 ( float ) 1500.0f );

            struct torque_spoof_t torque_spoof;

            calculate_torque_spoof( control, &torque_spoof );

            torque_sum = (uint8_t) ( torque_spoof.low + torque_spoof.high );

            dac.outputA( torque_spoof.low );
            dac.outputB( torque_spoof.high );
        }
        else
        {
            steering_module.override_flags.wheel = 0;

            pid_zeroize( &pid, steering_module.params.windup_guard );
        }
    }
}
