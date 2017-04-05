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

// Source for testing steering control ECU firmware
// 2014 Kia Soul Motor Driven Power Steering (MDPS) system
#include <SPI.h>
#include "mcp_can.h"
#include "can_frame.h"
#include "control_protocol_can.h"
#include "current_control_state.h"
#include "PID.h"
#include "common.h"
#include "DAC_MCP49xx.h"
#include "steering_control.h"
#include "test_steering_control.h"

#define PSYNC_DEBUG_FLAG ( true )

#ifdef PSYNC_DEBUG_FLAG
    #define DEBUG_PRINT( x )  Serial.println( x )
    #define STATIC
#else
    #define DEBUG_PRINT( x )
    #define STATIC static
#endif

// *****************************************************
// Function:    test_pid_values
//
// Purpose:     Test that the PID values for steering control haven't
//				been changed by the user
//
// Returns:     void
//
// Parameters:  [in] pid_params - the PID structure
//
// *****************************************************
bool test_pid_values( PID &pid_params, current_control_state &current_ctrl_state )
{
	bool return_code = false;

    if ( pid_params.proportional_gain == current_ctrl_state.SA_Kp &&
    	 pid_params.integral_gain == current_ctrl_state.SA_Ki &&
    	 pid_params.derivative_gain == current_ctrl_state.SA_Kd )
    {
    	return_code = true;
    }

    if( return_code == false )
    {
    	DEBUG_PRINT( "***************************************************************" );
    	DEBUG_PRINT( "****************************WARNING****************************" );
    	DEBUG_PRINT( "Default PID values have been changed." );
    	DEBUG_PRINT( "This can cause vehicle faults." );
    	DEBUG_PRINT( "Return the values to their default or proceed at your own risk." );
    	DEBUG_PRINT( "***************************************************************" );
    }
    return return_code;
}