/**
 * @file kia_soul.h
 * @brief Kia Soul specific macros.
 *
 */


#ifndef _KIA_SOUL_PLATFORM_INFO_H_
#define _KIA_SOUL_PLATFORM_INFO_H_


#include <stdint.h>


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

/*
 * @brief Minimum accumulator presure. [decibars]
 *
 */
#define ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS ( 777.6 )

/*
 * @brief Maximum accumulator pressure. [decibars]
 *
 */
#define ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS ( 878.0 )

/*
 * @brief Value of brake pressure that indicates operator override. [decibars]
 *
 */
#define DRIVER_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS ( 43.2 )

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
 * @brief Minimum clamped PID value of the actuation solenoid.
 *
 */
#define PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN ( 10.0 )

/*
 * @brief Maximum clamped PID value of the actuation solenoid.
 *
 */
#define PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX ( 110.0 )

/*
 * @brief Minimum clamped PID value of the release solenoid.
 *
 */
#define PID_RELEASE_SOLENOID_CLAMPED_MIN ( 0.0 )

/*
 * @brief Maximum clamped PID value of the release solenoid.
 *
 */
#define PID_RELEASE_SOLENOID_CLAMPED_MAX ( 60.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the actuation solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN ( 80.0 )

/*
 * @brief Maximum duty cycle where actuation solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX ( 105.0 )

/*
 * @brief Minimum duty cycle that begins to actuate the release solenoid.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define RELEASE_SOLENOID_DUTY_CYCLE_MIN ( 65.0 )

/*
 * @brief Maximum duty cycle where release solenoid has reached its stop.
 *
 * 3.921 KHz PWM frequency
 *
 */
#define RELEASE_SOLENOID_DUTY_CYCLE_MAX ( 100.0 )

/*
 * @brief Minimum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_SIGNAL_RANGE_MIN ( 868.0 )

/*
 * @brief Maximum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_SIGNAL_RANGE_MAX ( 3031.0 )

/*
 * @brief Value of the torque sensor that indicates operator override.
 *        [degrees/microsecond]
 *
 */
#define OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC ( 750 )

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

/*
 * @brief Minimum allowed value for the high spoof signal value.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 0 )

/*
 * @brief Maximum allowed value for the high spoof signal value.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 3500 )

/*
 * @brief Value of the accelerator position that indicates operator override.
 *
 */
#define ACCELERATOR_OVERRIDE_THRESHOLD ( 185.0 )

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
 * @brief Minimum allowable brake value.
 *
 */
#define MINIMUM_THROTTLE_COMMAND ( 0 )

/*
 * @brief Maximum allowable brake value.
 *
 */
#define MAXIMUM_THROTTLE_COMMAND ( 19660 )

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
 * @brief Calculation to convert a throttle position to a high spoof value.
 *
 */
#define THROTTLE_POSITION_TO_SPOOF_HIGH( position ) ( (position) )

/*
 * @brief Calculation to convert a throttle position to a low spoof value.
 *
 */
#define THROTTLE_POSITION_TO_SPOOF_LOW( position ) ( (position) )

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
 * @brief Calculation to convert a steering torque to a high spoof value.
 *
 */
#define STEERING_TORQUE_TO_SPOOF_HIGH( torque ) ( (torque) )

/*
 * @brief Calculation to convert a steering torque to a low spoof value.
 *
 */
#define STEERING_TORQUE_TO_SPOOF_LOW( torque ) ( (torque) )


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


#endif
