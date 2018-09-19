/**
 * @file kia_niro.h
 * @brief Kia Niro specific macros.
 *
 */


#ifndef _KIA_NIRO_PLATFORM_INFO_H_
#define _KIA_NIRO_PLATFORM_INFO_H_


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
 * @brief ID of the Kia Niro's OBD steering wheel angle CAN frame.
 *
 */
#define KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID ( 0x2B0 )

/*
 * @brief ID of the Kia Niro's OBD wheel speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID ( 0x386 )

/*
 * @brief ID of the Kia Niro's OBD brake pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID ( 0x220 )

/*
 * @brief ID of the Kia Niro's OBD throttle pressure CAN frame.
 *
 */
#define KIA_SOUL_OBD_THROTTLE_PRESSURE_CAN_ID ( 0x371 )

/*
 * @brief ID of the Kia Niro's OBD speed CAN frame.
 *
 */
#define KIA_SOUL_OBD_SPEED_CAN_ID ( 0x52A )

/*
 * @brief Factor to scale OBD steering angle to degrees
 *
 */
#define KIA_SOUL_OBD_STEERING_ANGLE_SCALAR ( 0.1 )

/**
 * @brief Steering wheel angle message data.
 * @warn Deprecated. Use \ref get_steering_wheel_angle instead.
 */
typedef struct
{
    int16_t steering_wheel_angle; /* 1/10 degrees */

    uint8_t reserved[6]; /* Reserved. */
} kia_soul_obd_steering_wheel_angle_data_s;

/**
 * @brief Wheel speed message data.
 * @warn Deprecated.
 * @warn Does not reflect CAN message data. Use the following functions instead:
 * \li \ref get_wheel_speed_right_rear
 * \li \ref get_wheel_speed_left_rear
 * \li \ref get_wheel_speed_right_front
 * \li \ref get_wheel_speed_left_front
 */
typedef struct
{
    uint8_t wheel_speed_front_left; /* 1/4 kph */

    uint8_t reserved_0; /* Reserved. */

    uint8_t wheel_speed_front_right; /* 1/4 kph */

    uint8_t reserved_1; /* Reserved. */

    uint8_t wheel_speed_rear_left; /* 1/4 kph */

    uint8_t reserved_2; /* Reserved. */

    uint8_t wheel_speed_rear_right; /* 1/4 kph */

    uint8_t reserved_3; /* Reserved. */
} kia_soul_obd_wheel_speed_data_s;

/**
 * @brief Brake pressure message data.
 * @warn Deprecated.
 * @warn Does not reflect CAN message data. Use \ref get_wheel_brake_pressure instead.
 */
typedef struct
{
    uint8_t reserved_0[4]; /* Reserved. */

    int16_t master_cylinder_pressure; /* 1/10th of a bar per bit */

    uint8_t reserved_1[2]; /* Reserved. */
} kia_soul_obd_brake_pressure_data_s;

/**
 * @brief Speed message data.
 *
 */
typedef struct
{
	uint8_t reserved_0[3];

    int16_t vehicle_speed; /* kph */

    uint8_t reserved_1[3];
} kia_soul_obd_speed_data_s;


// ****************************************************************************
// VEHICLE AND BOARD PARAMETERS
// ****************************************************************************

 /*
 * @brief Number of steps per volt corresponding to 4096 steps (2^12) across 5 volts.
 *
 */
#define STEPS_PER_VOLT ( 819.2 )

/*
 * @brief Length of time in ms for delay of signal reads to ensure fault is
 * outside the range of noise in the signal.
 *
 */
#define FAULT_HYSTERESIS ( 150 )




// ****************************************************************************
// BRAKE MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable brake value.
 *
 */
#define MINIMUM_BRAKE_COMMAND ( 0.0 )

/*
 * @brief Maximum allowable brake value.
 *
 */
#define MAXIMUM_BRAKE_COMMAND ( 1.0 )

/*
 * @brief Minimum allowed voltage for the high spoof signal voltage. [volts]
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN ( 0.609 )

/*
 * @brief Maximum allowed voltage for the high spoof signal voltage. [volts]
 *
 */
#define BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX ( 2.880 )

/**
 * @brief Minimum allowed voltage for the low spoof signal voltage. [volts]
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN ( 0.279 )

/**
 * @brief Maximum allowed voltage for the low spoof signal voltage. [volts]
 *
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX ( 1.386 )

/*
 * @brief Minimum allowed value for the high spoof signal value. [steps]
 *
 * Equal to \ref BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define BRAKE_SPOOF_LOW_SIGNAL_RANGE_MIN ( 499 )

/*
 * @brief Minimum allowed value for the high spoof signal value. [steps]
 *
 * Equal to \ref BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define BRAKE_SPOOF_LOW_SIGNAL_RANGE_MAX ( 2359 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 229 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define BRAKE_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 1135 )

/*
 * @brief Calculation to convert a brake position to a low spoof voltage.
 *
 */
#define BRAKE_POSITION_TO_VOLTS_LOW( position ) ( (position) *\
    (BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX - BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN) +\
    BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN  )

/*
 * @brief Calculation to convert a brake position to a high spoof voltage.
 *
 */
#define BRAKE_POSITION_TO_VOLTS_HIGH( position ) ( (position) *\
    (BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX - BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN) +\
    BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN  )

/*
 * @brief Value of the brake position that indicates operator override. [steps]
 *
 */
#define BRAKE_PEDAL_OVERRIDE_THRESHOLD ( 130 )

/*
 * @brief Minimum value of the high spoof signal that activates the brake lights. [steps]
 *
 */
#define BRAKE_LIGHT_SPOOF_HIGH_THRESHOLD ( 300 )

/*
 * @brief Minimum value of the low spoof signal that activates the brake lights. [steps]
 *
 */
#define BRAKE_LIGHT_SPOOF_LOW_THRESHOLD ( 600 )




// ****************************************************************************
// STEERING MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable torque value.
 *
 */
#define MINIMUM_TORQUE_COMMAND ( -12.8 )

/*
 * @brief Maximum allowable torque value.
 *
 */
#define MAXIMUM_TORQUE_COMMAND ( 12.7 )

/*
 * @brief Minimum allowable steering DAC output. [volts]
 *
 */
#define STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MIN ( 0.80 )

/*
 * @brief Maximum allowable steering DAC output. [volts]
 *
 */
#define STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MAX ( 4.10 )

/*
 * @brief Minimum allowable steering DAC output. [volts]
 *
 */
#define STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN ( 0.90 )

/*
 * @brief Maximum allowable steering DAC output. [volts]
 *
 */
#define STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX ( 4.20 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define STEERING_SPOOF_LOW_SIGNAL_RANGE_MIN ( 656 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define STEERING_SPOOF_LOW_SIGNAL_RANGE_MAX ( 3358 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define STEERING_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 738 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define STEERING_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 3440 )

/*
 * @brief Scalar value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_SCALE ( 0.135 )

/*
 * @brief Offset value for the low spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_LOW_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.39 )

/*
 * @brief Scalar value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_SCALE ( -0.145 )

/*
 * @brief Offset value for the high spoof signal taken from a calibration curve.
 *
 */
#define TORQUE_SPOOF_HIGH_SIGNAL_CALIBRATION_CURVE_OFFSET ( 2.42 )

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
 * @brief Value of torque sensor difference that indicates likely operator
 *        override.
 *
 */
#define TORQUE_DIFFERENCE_OVERRIDE_THRESHOLD ( 2000 )




// ****************************************************************************
// THROTTLE MODULE
// ****************************************************************************

/*
 * @brief Minimum allowable throttle value.
 *
 */
#define MINIMUM_THROTTLE_COMMAND ( 0.0 )

/*
 * @brief Maximum allowable throttle value.
 *
 */
#define MAXIMUM_THROTTLE_COMMAND ( 1.0 )

/*
 * @brief Minimum allowed voltage for the low spoof signal voltage. [volts]
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN ( 0.380 )

/*
 * @brief Maximum allowed voltage for the low spoof signal voltage. [volts]
 *
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX ( 2.104 )

/**
 * @brief Minimum allowed voltage for the high spoof signal voltage. [volts]
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN ( 0.757 )

/**
 * @brief Maximum allowed voltage for the high spoof signal voltage. [volts]
 *
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX ( 4.207 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MIN ( 311 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define THROTTLE_SPOOF_LOW_SIGNAL_RANGE_MAX ( 1723 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN * \ref STEPS_PER_VOLT.
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MIN ( 620 )

/*
 * @brief Minimum allowed value for the low spoof signal value. [steps]
 *
 * Equal to \ref THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX * \ref STEPS_PER_VOLT.
 */
#define THROTTLE_SPOOF_HIGH_SIGNAL_RANGE_MAX ( 3446 )

/*
 * @brief Calculation to convert a throttle position to a low spoof voltage.
 *
 */
#define THROTTLE_POSITION_TO_VOLTS_LOW( position ) ( (position) *\
    (THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN) +\
    THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN  )

/*
 * @brief Calculation to convert a throttle position to a high spoof voltage.
 *
 */
#define THROTTLE_POSITION_TO_VOLTS_HIGH( position ) ( (position) *\
    (THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX - THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN) +\
    THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN  )

/*
 * @brief Value of the accelerator position that indicates operator override. [steps]
 *
 */
#define ACCELERATOR_OVERRIDE_THRESHOLD ( 185.0 )



#endif
