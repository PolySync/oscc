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
    double steering_angle_rate_max = 1000.0; /* Maximum rate of change of steering wheel angle */
    //
    //    
    double steering_angle_last; /* Last steering angle recorded */
    //
    //
    uint32_t timestamp_ms; /* Keeps track of last control loop time in us */
} current_control_state;
