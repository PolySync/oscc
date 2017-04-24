/************************************************************************/
/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */
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

#include <SPI.h>
#include "mcp_can.h"
#include "gateway_protocol_can.h"
#include "brake_protocol_can.h"
#include "PID.h"
#include "serial.h"
#include "can.h"
#include "time.h"
#include "debug.h"

#include "globals.h"
#include "accumulator.h"
#include "helper.h"
#include "master_cylinder.h"
#include "brake_control.h"
#include "communications.h"


void setup( void )
{
    // set the Arduino's PWM timers to 3.921 KHz, above the acoustic range
    TCCR3B = (TCCR3B & 0xF8) | 0x02; // pins 2,3,5 | timer 3
    TCCR4B = (TCCR4B & 0xF8) | 0x02; // pins 6,7,8 | timer 4

    accumulator_init( );
    master_cylinder_init( );
    brake_init( );

    // depower all the things
    accumulator_turn_pump_off( );
    master_cylinder_open( );

    brake_command_release_solenoids( 0 );
    brake_command_actuator_solenoids( 0 );

    #ifdef DEBUG
        init_serial( );
    #endif

    init_can( can );

    publish_brake_report( );

    // update last Rx timestamps so we don't set timeout warnings on start up
    brake_control_state.rx_timestamp = GET_TIMESTAMP_MS( );

    DEBUG_PRINTLN( "init: pass" );
}


void loop( )
{
    can_frame_s rx_frame;
    int ret = check_for_rx_frame( can, &rx_frame );

    if( ret == RX_FRAME_AVAILABLE )
    {
        handle_ready_rx_frames( &rx_frame );
    }

    publish_timed_tx_frames( );

    accumulator_maintain_pressure( );

    check_rx_timeouts( );

    brake_check_driver_override( );

    if ( brake_control_state.enabled != brake_control_state.enable_request )
    {
        if ( brake_control_state.enable_request == true )
        {
            brake_enable( );
        }
        else
        {
            brake_disable( );
        }
    }

    if ( brake_control_state.enabled == true )
    {
        brake_update( );
    }
}
