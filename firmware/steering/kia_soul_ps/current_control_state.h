/************************************************************************/
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

/**
 * @brief Current control state.
 *
 * Keeps track of what state the arduino controller is currently in.
 *
 */




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
    double SA_Kp = 0.32; /* Proportional gain for PID controller */
    //
    //
    double SA_Ki = 2.0; /* Integral gain for PID controller */
    //
    //    
    double SA_Kd = 0.03; /* Derivative gain for PID controller */
    //
    //
    double steering_angle_rate_max = 1000; /* Maximum rate of change of steering wheel angle */
    //
    //    
    double steering_angle_last; /* Last steering angle recorded */
    //
    //
    long unsigned int lastMicros; /* Keeps track of last control loop time */
} current_control_state;
