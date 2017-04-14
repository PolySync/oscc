#ifndef _OSCC_KIA_SOUL_STEERING_PARAMS_H_
#define _OSCC_KIA_SOUL_STEERING_PARAMS_H_


/*******************************************************************************
*   WARNING
*
*   The PID gains (SA_Kp, SA_Ki, SA_Kd) are carefully tested to ensure that a
*   torque is not requested that the vehicles steering motor cannot handle.
*   By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/
typedef struct
{    //
    //
    double steering_angle_rate_max = 1000.0; /* Maximum rate of change of steering wheel angle */
    //
    //
    double steering_wheel_cutoff_threshold = 3000;
    //
    //
    double SA_Kp = 0.3; /* Proportional gain for PID controller */
    //
    //
    double SA_Ki = 1.3; /* Integral gain for PID controller */
    //
    //
    double SA_Kd = 0.03; /* Derivative gain for PID controller */
} kia_soul_steering_params_s;


#endif
