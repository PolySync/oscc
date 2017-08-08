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

/*
 * @brief Factor to scale OBD steering angle to degrees
 *
 */
#define KIA_SOUL_OBD_STEERING_ANGLE_SCALAR ( 0.1 )

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
 * @brief Calculation to convert a throttle position to a high spoof value.
 *
 */
#define BRAKE_POSITION_TO_SPOOF_HIGH( position ) (\
            ((BRAKE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
            + BRAKE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Calculation to convert a throttle position to a low spoof value.
 *
 */
#define BRAKE_POSITION_TO_SPOOF_LOW( position ) (\
            ((BRAKE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
            + BRAKE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Minimum allowed value for the low spoof signal value.
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN ( 248 )

/*
 * @brief Maximum allowed value for the low spoof signal value.
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX ( 1147 )

/**
 * @brief Wheel speed message data.
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 520 )

/**
 * @brief Wheel speed message data.
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 2351 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE (0.000021)

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET (0.303)

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE (0.000043)

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET (0.635)

/*
 * @brief Value of the accelerator position that indicates operator override.
 *
 */
#define BRAKE_PEDAL_OVERRIDE_THRESHOLD ( 130.0 )




// ****************************************************************************
// STEERING MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN ( 1000 )

/*
 * @brief Maximum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX ( 2965 )

/*
 * @brief Minimum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 1030 )

/*
 * @brief Maximum allowable steering DAC output. [steps]
 *
 */
#define STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 2995 )

/*
 * @brief Minimum allowable torque value.
 *
 */
#define MINIMUM_TORQUE_COMMAND ( -10 )

/*
 * @brief Maximum allowable torque value.
 *
 */
#define MAXIMUM_TORQUE_COMMAND ( 10 )


 /*
 * @brief Number of steps per volt corresponding to 4096 steps across 5 volts.
 *
 */
#define STEPS_PER_VOLT ( 819.2 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE ( 0.12 )

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.42 )

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE ( -0.12 )

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.46 )

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
#define STEERING_TORQUE_TO_VOLTS_HIGH( torque ) (\
            ((TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * (torque))\
            + TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Calculation to convert a steering torque to a low spoof value.
 *
 */
#define STEERING_TORQUE_TO_VOLTS_LOW( torque ) (\
            ((TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * (torque))\
            + TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET))

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
#define MAXIMUM_THROTTLE_COMMAND ( 820 )

/*
 * @brief Calculation to convert a throttle position to a high spoof value.
 *
 */
#define THROTTLE_POSITION_TO_VOLTS_HIGH( position ) (\
            ((THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
            + THROTTLE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET))

/*
 * @brief Calculation to convert a throttle position to a low spoof value.
 *
 */
#define THROTTLE_POSITION_TO_VOLTS_LOW( position ) (\
            ((THROTTLE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE * (position))\
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
