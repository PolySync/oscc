
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
    bool control_enabled; /*Is control currently enabled*/
	//
	//
	bool emergency_stop;
	//
	//
	int16_t commanded_torque;
	//
	//
	double current_steering_angle;
	//
	//
	double commanded_steering_angle;
} current_control_state;
