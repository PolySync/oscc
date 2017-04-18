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

// Throttle control ECU firmware
// 2014 Kia Soul throttle system


#include <SPI.h>
#include "mcp_can.h"
#include "control_protocol_can.h"
#include "DAC_MCP49xx.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "throttle_module.h"
#include "init.h"
#include "communications.h"
#include "throttle_control.h"

static kia_soul_throttle_module_s throttle_module;
static DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, throttle_module.pins.dac_cs ); // DAC model, SS pin, LDAC pin
static MCP_CAN can(throttle_module.pins.can_cs); // Set CS pin for the CAN shield
static can_frame_s rx_frame_ps_ctrl_throttle_command;
static can_frame_s tx_frame_ps_ctrl_throttle_report;

void setup( )
{
    memset( &rx_frame_ps_ctrl_throttle_command,
            0,
            sizeof( rx_frame_ps_ctrl_throttle_command ) );

    init_pins( &throttle_module );

    #ifdef DEBUG
    init_serial( );
    #endif

    init_can( can );

    publish_ps_ctrl_throttle_report( &throttle_module,
        &tx_frame_ps_ctrl_throttle_report,
        can );

    throttle_module.control_state.enabled = false;

    throttle_module.control_state.emergency_stop = false;

    throttle_module.override_flags.pedal = 0;

    throttle_module.override_flags.voltage = 0;

    throttle_module.override_flags.voltage_spike_a = 0;

    throttle_module.override_flags.voltage_spike_b = 0;

    // update last Rx timestamps so we don't set timeout warnings on start up
    rx_frame_ps_ctrl_throttle_command.timestamp = GET_TIMESTAMP_MS( );

    // debug log
    DEBUG_PRINTLN( "init: pass" );
}

void loop()
{
    // checks for CAN frames, if yes, updates state variables
    can_frame_s rx_frame;
    int ret = check_for_rx_frame( can, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
        handle_ready_rx_frames( &throttle_module, &rx_frame, &rx_frame_ps_ctrl_throttle_command, dac );
    }

    // publish all report CAN frames
    publish_timed_report(
        &throttle_module,
        &tx_frame_ps_ctrl_throttle_report,
        can );

    // heartbeat checker??
    check_rx_timeouts( &throttle_module, &rx_frame_ps_ctrl_throttle_command, dac );

    // update state variables
    throttle_module.state.accel_position_sensor_low = analogRead( throttle_module.pins.signal_accel_pos_sensor_high ) << 2;  //10 bit to 12 bit
    throttle_module.state.accel_position_sensor_high = analogRead( throttle_module.pins.signal_accel_pos_sensor_low ) << 2;

    // if someone is pressing the throttle pedal, disable control
    check_pedal_override( &throttle_module, dac );

    // now that we've set control status, do throttle if we are in control
    if ( throttle_module.control_state.enabled == true )
    {

        struct torque_spoof_t torque_spoof;

        calculate_pedal_spoof(
                throttle_module.state.accel_position_target,
                &torque_spoof );

        dac.outputA( torque_spoof.high );
        dac.outputB( torque_spoof.low );
    }
}
