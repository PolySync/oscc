/**
 * @file obd_can_protocol.h
 * @brief Kia Soul OBD-II CAN Protocol.
 *
 */


#ifndef _KIA_SOUL_PLATFORM_INFO_H_
#define _KIA_SOUL_PLATFORM_INFO_H_


#include <stdint.h>


/*
 * @brief ID of the Kia Soul's OBD steering wheel angle CAN frame.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID (0x2B0)

/*
 * @brief ID of the Kia Soul's OBD wheel speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID (0x4B0)

/*
 * @brief ID of the Kia Soul's OBD brake pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID (0x220)

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
#define BRAKE_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS (20.0)

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
 * @brief Scalar value to convert angle reported by OBD to human-readable value.
 *
 */
#define RAW_ANGLE_SCALAR ( 0.0076294 )

/*
 * @brief Scalar value to convert wheel angle (-40 to 40 degrees) to steering
 *        wheel angle (-470 to 470) degrees.
 *
 */
#define WHEEL_ANGLE_TO_STEERING_WHEEL_ANGLE_SCALAR ( 11.7 )

/*
 * @brief Minimum steering angle rate. [Newton meters]
 *
 */
#define TORQUE_MIN_IN_NEWTON_METERS ( -1500.0 )

/*
 * @brief Maximum steering angle rate. [Newton meters]
 *
 */
#define TORQUE_MAX_IN_NEWTON_METERS ( 1500.0 )

/*
 * @brief Value of the torque sensor that indicates operator override.
 *        [degrees/microsecond]
 *
 */
#define OVERRIDE_WHEEL_THRESHOLD_IN_DEGREES_PER_USEC ( 3000 )

/*
 * @brief Number of steps per volt corresponding to 4096 steps across 5 volts.
 *
 */
#define STEPS_PER_VOLT ( 819.2 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALAR ( 0.0008 )

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.26 )

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALAR ( -0.0008 )

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.5 )

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
 * @brief Minimum allowed value for the low spoof signal value.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN (0)

/*
 * @brief Maximum allowed value for the low spoof signal value.
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX (1800)

/*
 * @brief Minimum allowed value for the high spoof signal value.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN (0)

/*
 * @brief Maximum allowed value for the high spoof signal value.
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX (3500)

/*
 * @brief Value of the accelerator position that indicates operator override.
 *
 */

#define ACCELERATOR_OVERRIDE_THRESHOLD ( 185.0 )


/**
 * @brief Steering wheel angle message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t steering_wheel_angle; /* 1/10th of a degree per bit. */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_steering_wheel_angle_data_s;


/**
 * @brief Wheel speed message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t wheel_speed_front_left; /* 1/128 mph per bit */

    int16_t wheel_speed_front_right; /* 1/128 mph per bit */

    int16_t wheel_speed_rear_left; /* 1/128 mph per bit */

    int16_t wheel_speed_rear_right; /* 1/128 mph per bit */
} kia_soul_obd_wheel_speed_data_s;


/**
 * @brief Brake pressure message data.
 *
 * Message size (CAN frame DLC): 8
 *
 */
typedef struct
{
    int16_t master_cylinder_pressure; /* 1/10th of a bar per bit */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_brake_pressure_data_s;


#endif
