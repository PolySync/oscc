/**
 * @file kia_soul.h
 * @brief Kia Soul specific macros.
 *
 */


#ifndef _KIA_SOUL_PLATFORM_INFO_H_
#define _KIA_SOUL_PLATFORM_INFO_H_


#include <stdint.h>


// ********************************************************************
//
// WARNING
//
// The values listed here are carefully tested to ensure that the vehicle's
// components are not actuated outside of the range of what they can handle.
// By changing any of these values you risk attempting to actuate outside of the
// vehicle's valid range. This can cause damage to the hardware and/or a
// vehicle fault. Clearing this fault state requires additional tools.
//
// It is NOT recommended to modify any of these values without expert knowledge.
//
// ************************************************************************


// ****************************************************************************
// OBD MESSAGES
// ****************************************************************************

/*
 * @brief ID of the Kia Soul's OBD steering wheel angle CAN frame.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID ( 0x2B0 )

/*
 * @brief ID of the Kia Soul's OBD wheel speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID ( 0x4B0 )

/*
 * @brief ID of the Kia Soul's OBD brake pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID ( 0x220 )

/**
 * @brief Steering wheel angle message data.
 *
 */
typedef struct
{
    int16_t steering_wheel_angle; /* 1/10 degrees */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_steering_wheel_angle_data_s;

/**
 * @brief Wheel speed message data.
 *
 */
typedef struct
{
    int16_t wheel_speed_front_left; /* 1/50 mph */

    int16_t wheel_speed_front_right; /* 1/50 mph */

    int16_t wheel_speed_rear_left; /* 1/50 mph */

    int16_t wheel_speed_rear_right; /* 1/50 mph */
} kia_soul_obd_wheel_speed_data_s;

/**
 * @brief Brake pressure message data.
 *
 */
typedef struct
{
    int16_t master_cylinder_pressure; /* 1/10th of a bar per bit */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_brake_pressure_data_s;




// ****************************************************************************
// BRAKE MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable brake value.
 *
 */
#define MINIMUM_BRAKE_COMMAND ( 0 )

/*
 * @brief Maximum allowable brake value.
 *
 */
#define MAXIMUM_BRAKE_COMMAND ( 52428 )

/*
 * @brief Calculation to convert a brake position to a pedal position.
 *
 */
#define BRAKE_POSITION_TO_PEDAL( position ) ( (position) )

/*
 * @brief Calculation to convert a brake pressure to a pedal position.
 *
 */
#define BRAKE_PRESSURE_TO_PEDAL( pressure ) ( (pressure) )

/*
 * @brief Minimum accumulator presure. [decibars]
 *
 */
#define BRAKE_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS ( 777.6 )

/*
 * @brief Maximum accumulator pressure. [decibars]
 *
 */
#define BRAKE_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS ( 878.0 )

/*
 * @brief Value of brake pressure that indicates operator override. [decibars]
 *
 */
#define BRAKE_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ( 43.2 )

/*
 * @brief Brake pressure threshold for when to enable the brake light.
 *
 */
#define BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS ( 20.0 )

/*
 * @brief Minimum possible pressure of brake system. [decibars]
 *
 */
#define BRAKE_PRESSURE_MIN_IN_DECIBARS ( 12.0 )

/*
 * @brief Maximum possible pressure of brake system. [decibars]
 *
 */
#define BRAKE_PRESSURE_MAX_IN_DECIBARS ( 878.3 )

/*
 * @brief Minimum possible value expected to be read from the brake pressure
 * sensors when the pressure check pins (PCK1/PCK2) are asserted.
 *
 */
#define BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MIN ( 665 )

/*
 * @brief Maximum possible value expected to be read from the brake pressure
 * sensors when the pressure check pins (PCK1/PCK2) are asserted.
 *
 */
#define BRAKE_PRESSURE_SENSOR_CHECK_VALUE_MAX ( 680 )

/*
 * @brief Proportional gain of the PID controller.
 *
 */
#define BRAKE_PID_PROPORTIONAL_GAIN ( 0.65 )

/*
 * @brief Integral gain of the PID controller.
 *
 */
#define BRAKE_PID_INTEGRAL_GAIN ( 1.75 )

/*
 * @brief Derivative gain of the PID controller.
 *
 */
#define BRAKE_PID_DERIVATIVE_GAIN ( 0.001 )

/*
 * @brief Windup guard of the PID controller.
 *
 */
#define BRAKE_PID_WINDUP_GUARD ( 30 )

/*
 * @brief Minimum output value of PID to be within a valid pressure range.
 *
 */
#define BRAKE_PID_OUTPUT_MIN ( -10.0 )

/*
 * @brief Maximum output value of PID to be within a valid pressure range.
 *
 */
#define BRAKE_PID_OUTPUT_MAX ( 10.0 )

/*
 * @brief Minimum clamped PID value of the actuation solenoid.
 *
 */
#define BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN ( 10.0 )

/*
 * @brief Maximum clamped PID value of the actuation solenoid.
 *
 */
#define BRAKE_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX ( 110.0 )

/*
 * @brief Minimum clamped PID value of the release solenoid.
 *
 */
#define BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MIN ( 0.0 )

/*
 * @brief Maximum clamped PID value of the release solenoid.
 *
 */
#define BRAKE_PID_RELEASE_SOLENOID_CLAMPED_MAX ( 60.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the actuation solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN ( 80.0 )

/*
 * @brief Maximum duty cycle where actuation solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define BRAKE_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX ( 105.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the release solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MIN ( 65.0 )

/*
 * @brief Maximum duty cycle where release solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define BRAKE_RELEASE_SOLENOID_DUTY_CYCLE_MAX ( 100.0 )




// ****************************************************************************
// STEERING MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_SIGNAL_MIN ( 868.0 )

/*
 * @brief Maximum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_SIGNAL_MAX ( 3031.0 )

/*
 * @brief Minimum torque value [Nm]
 *
 */
 #define STEERING_TORQUE_MIN ( -1500.0 )

 /*
 * @brief Maximum torque value [Nm]
 *
 */
 #define STEERING_TORQUE_MAX ( 1500.0 )

 /*
 * @brief Number of steps per volt corresponding to 4096 steps across 5 volts.
 *
 */
#define STEPS_PER_VOLT ( 819.2 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SCALAR ( 0.12 )

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_OFFSET ( 2.26 )

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SCALAR ( -0.12 )

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_OFFSET ( 2.5 )

/*
 * @brief Calculation to convert a steering angle to a high spoof value.
 *
 */
#define STEERING_ANGLE_TO_SPOOF_HIGH( angle ) ( (angle) )

/*
 * @brief Calculation to convert a steering angle to a low spoof value.
 *
 */
#define STEERING_ANGLE_TO_SPOOF_LOW( angle ) ( (angle) )

/*
 * @brief Minimum allowed value for the high spoof signal value.
 *
 */
#define STEERING_TORQUE_TO_SPOOF_HIGH( torque ) ( STEPS_PER_VOLT * ((TORQUE_SPOOF_HIGH_SCALAR * torque) + TORQUE_SPOOF_HIGH_OFFSET) )

/*
 * @brief Calculation to convert a steering torque to a low spoof value.
 *
 */
#define STEERING_TORQUE_TO_SPOOF_LOW( torque ) ( STEPS_PER_VOLT * ((TORQUE_SPOOF_LOW_SCALAR * torque) + TORQUE_SPOOF_LOW_OFFSET) )

/*
 * @brief Value of the torque sensor that indicates operator override.
 *        [degrees/microsecond]
 *
 */
#define OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC ( 750 )




// ****************************************************************************
// THROTTLE MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable throttle value.
 *
 */
#define MINIMUM_THROTTLE_COMMAND ( 0 )

/*
 * @brief Maximum allowable throttle value.
 *
 */
#define MAXIMUM_THROTTLE_COMMAND ( 19660 )

/*
 * @brief Calculation to convert a throttle position to a high spoof value.
 *
 */
#define THROTTLE_POSITION_TO_SPOOF_HIGH( position ) (STEPS_PER_VOLT\
            * ((THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
            + THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Calculation to convert a throttle position to a low spoof value.
 *
 */
#define THROTTLE_POSITION_TO_SPOOF_LOW( position ) (STEPS_PER_VOLT\
            * ((THROTTLE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
            + THROTTLE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Minimum allowed value for the low spoof signal value.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN ( 0 )

/*
 * @brief Maximum allowed value for the low spoof signal value.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX ( 1800 )

/**
 * @brief Wheel speed message data.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 0 )

/**
 * @brief Wheel speed message data.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 3500 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE (0.0004)

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET (0.366)

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE (0.0008)

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET (0.732)

/*
 * @brief Value of the accelerator position that indicates operator override.
 *
 */
#define ACCELERATOR_OVERRIDE_THRESHOLD ( 185.0 )



#endif
