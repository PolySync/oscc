/**
 * @file global_can_protocol.h
 * @brief Global CAN Protocol.
 *
 */


#ifndef _OSCC_GLOBAL_CAN_PROTOCOL_H_
#define _OSCC_GLOBAL_CAN_PROTOCOL_H_


#include <stdint.h>
#include "magic.h"


/*
 * @brief CAN ID representing the range of global messages.
 *
 */
#define OSCC_GLOBAL_CAN_ID_INDEX (0x90)

/*
 * @brief U8 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_U8_CAN_ID (0x90)

/*
 * @brief U16 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_U16_CAN_ID (0x91)

/*
 * @brief U16 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_U32_CAN_ID (0x92)

/*
 * @brief I8 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_I8_CAN_ID (0x93)

/*
 * @brief I16 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_I16_CAN_ID (0x94)

/*
 * @brief I32 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_I32_CAN_ID (0x95)

/*
 * @brief F32 configuration message CAN ID.
 *
 */
#define OSCC_CONFIG_F32_CAN_ID (0x96)

/*
 * @brief Fault report message (CAN frame) ID.
 *
 */
#define OSCC_FAULT_REPORT_CAN_ID (0x9F)

/*
 * @brief Fault report message (CAN frame) length.
 *
 */
#define OSCC_FAULT_REPORT_CAN_DLC (8)

/*
 * @brief Beginning address in EEPROM where U8 configuration values are stored.
 *
 * Range: 0x000 to 0x01F
 * Capacity: 32
 *
 */
#define OSCC_CONFIG_U8_EEPROM_INDEX (0x000)

/*
 * @brief Beginning address in EEPROM where U16 configuration values are stored.
 *
 * Range: 0x020 to 0x09F
 * Capacity: 64
 *
 */
#define OSCC_CONFIG_U16_EEPROM_INDEX (0x020)

/*
 * @brief Beginning address in EEPROM where U32 configuration values are stored.
 *
 * Range: 0x0A0 to 0x19F
 * Capacity: 64
 *
 */
#define OSCC_CONFIG_U32_EEPROM_INDEX (0x0A0)

/*
 * @brief Beginning address in EEPROM where I8 configuration values are stored.
 *
 * Range: 0x1A0 to 0x1BF
 * Capacity: 32
 *
 */
#define OSCC_CONFIG_I8_EEPROM_INDEX (0x1A0)

/*
 * @brief Beginning address in EEPROM where I16 configuration values are stored.
 *
 * Range: 0x1C0 to 0x23F
 * Capacity: 64
 *
 */
#define OSCC_CONFIG_I16_EEPROM_INDEX (0x1C0)

/*
 * @brief Beginning address in EEPROM where I32 configuration values are stored.
 *
 * Range: 0x240 to 0x33F
 * Capacity: 64
 *
 */
#define OSCC_CONFIG_I32_EEPROM_INDEX (0x240)


/*
 * @brief Beginning address in EEPROM where F32 configuration values are stored.
 *
 * Range: 0x340 to 0x43F
 * Capacity: 64
 *
 */
#define OSCC_CONFIG_F32_EEPROM_INDEX (0x340)


/*
 * @brief Enumeration of all possible U8 configuration parameters.
 *
 */
typedef enum
{
    /* Number of possible U8 configuration parameters. */
    OSCC_CONFIG_U8_COUNT
} oscc_config_u8_name_t;


/*
 * @brief Enumeration of all possible U16 configuration parameters.
 *
 */
typedef enum
{
    /* Value of the brake pedal position sensor that indicates operator override. */
    OSCC_CONFIG_U16_BRAKE_EV_PEDAL_OVERRIDE_THRESHOLD = 0,

    /* Minimum value of the low spoof signal that activates the brake lights. */
    OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_LOW_THRESHOLD,

    /* Minimum value of the high spoof signal that activates the brake lights. */
    OSCC_CONFIG_U16_BRAKE_EV_LIGHT_SPOOF_HIGH_THRESHOLD,

    /* Frequency at which to check for faults in the EV brake module. */
    OSCC_CONFIG_U16_BRAKE_EV_FAULT_CHECK_FREQUENCY_IN_HZ,

    /* Minimum possible value expected to be read from the brake pressure
     * sensors when the pressure check pins (PCK1/PCK2) are asserted. */
    OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MIN,

    /* Maximum possible value expected to be read from the brake pressure
    * sensors when the pressure check pins (PCK1/PCK2) are asserted. */
    OSCC_CONFIG_U16_BRAKE_PETROL_PRESSURE_SENSOR_CHECK_VALUE_MAX,

    /* Frequency at which to check for faults in the petrol brake module. */
    OSCC_CONFIG_U16_BRAKE_PETROL_FAULT_CHECK_FREQUENCY_IN_HZ,

    /* Value of steering torque sensor difference that indicates likely operator override. */
    OSCC_CONFIG_U16_STEERING_TORQUE_OVERRIDE_THRESHOLD,

    /* Frequency at which to check for faults in the steering module. */
    OSCC_CONFIG_U16_STEERING_FAULT_CHECK_FREQUENCY_IN_HZ,

    /* Value of the throttle pedal position sensor that indicates operator override. */
    OSCC_CONFIG_U16_THROTTLE_PEDAL_OVERRIDE_THRESHOLD,

    /* Frequency at which to check for faults in the throttle module. */
    OSCC_CONFIG_U16_THROTTLE_FAULT_CHECK_FREQUENCY_IN_HZ,

    /* Number of possible U16 configuration parameters. */
    OSCC_CONFIG_U16_COUNT
} oscc_config_u16_name_t;

/*
 * @brief Enumeration of all possible U32 configuration parameters.
 *
 */
typedef enum
{
    /* Number of possible U32 configuration parameters. */
    OSCC_CONFIG_U32_COUNT
} oscc_config_u32_name_t;

/*
 * @brief Enumeration of all possible I8 configuration parameters.
 *
 */
typedef enum
{
    /* Number of possible I8 configuration parameters. */
    OSCC_CONFIG_I8_COUNT
} oscc_config_i8_name_t;

/*
 * @brief Enumeration of all possible I16 configuration parameters.
 *
 */
typedef enum
{
    /* Number of possible I16 configuration parameters. */
    OSCC_CONFIG_I16_COUNT
} oscc_config_i16_name_t;

/*
 * @brief Enumeration of all possible I32 configuration parameters.
 *
 */
typedef enum
{
    /* Number of possible I32 configuration parameters. */
    OSCC_CONFIG_I32_COUNT
} oscc_config_i32_name_t;

/*
 * @brief Enumeration of all possible F32 configuration parameters.
 *
 */
typedef enum
{
    /* Minimum allowed voltage for the low brake spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_BRAKE_EV_SPOOF_LOW_SIGNAL_VOLTAGE_MIN = 0,

    /* Maximum allowed voltage for the low brake spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_BRAKE_EV_SPOOF_LOW_SIGNAL_VOLTAGE_MAX,

    /* Minimum allowed voltage for the high brake spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_BRAKE_EV_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,

    /* Maximum allowed voltage for the high brake spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_BRAKE_EV_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX,

    /* Minimum accumulator pressure. */
    OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS,

    /* Maximum accumulator pressure. */
    OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS,

    /* Value of brake pressure that indicates operator override. */
    OSCC_CONFIG_F32_BRAKE_PETROL_OVERRIDE_PEDAL_THRESHOLD_IN_DECIBARS,

    /* Brake pressure threshold for when to enable the brake light. */
    OSCC_CONFIG_F32_BRAKE_PETROL_LIGHT_PRESSURE_THRESHOLD_IN_DECIBARS,

    /* Minimum possible pressure of brake system. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MIN_IN_DECIBARS,

    /* Maximum possible pressure of brake system. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PRESSURE_MAX_IN_DECIBARS,

    /* Proportional gain of the PID controller. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_PROPORTIONAL_GAIN,

    /* Integral gain of the PID controller. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_INTEGRAL_GAIN,

    /* Derivative gain of the PID controller. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_DERIVATIVE_GAIN,

    /* Windup guard of the PID controller. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_WINDUP_GUARD,

    /* Minimum output value of PID to be within a valid pressure range. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MIN,

    /* Maximum output value of PID to be within a valid pressure range. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_OUTPUT_MAX,

    /* Minimum clamped PID value of the actuation solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MIN,

    /* Maximum clamped PID value of the actuation solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_ACCUMULATOR_SOLENOID_CLAMPED_MAX,

    /* Minimum clamped PID value of the release solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MIN,

    /* Maximum clamped PID value of the release solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_PID_RELEASE_SOLENOID_CLAMPED_MAX,

    /* Minimum duty cycle that begins to actuate the actuation solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MIN,

    /* Maximum duty cycle where actuation solenoid has reached its stop. */
    OSCC_CONFIG_F32_BRAKE_PETROL_ACCUMULATOR_SOLENOID_DUTY_CYCLE_MAX,

    /* Minimum duty cycle that begins to actuate the release solenoid. */
    OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MIN,

    /* Maximum duty cycle where release solenoid has reached its stop. */
    OSCC_CONFIG_F32_BRAKE_PETROL_RELEASE_SOLENOID_DUTY_CYCLE_MAX,

    /* Minimum allowed voltage for the low steering spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MIN,

    /* Maximum allowed voltage for the low steering spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MAX,

    /* Minimum allowed voltage for the high steering spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,

    /* Maximum allowed voltage for the high steering spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX,

    /* Minimum allowed voltage for the low throttle spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN,

    /* Maximum allowed voltage for the low throttle spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX,

    /* Minimum allowed voltage for the high throttle spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,

    /* Maximum allowed voltage for the high throttle spoof signal voltage. [volts] */
    OSCC_CONFIG_F32_THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX,

    /* Number of possible F32 configuration parameters. */
    OSCC_CONFIG_F32_COUNT
} oscc_config_f32_name_t;


typedef enum
{
    FAULT_ORIGIN_BRAKE,
    FAULT_ORIGIN_STEERING,
    FAULT_ORIGIN_THROTTLE
} fault_origin_id_t;


#pragma pack(push)
#pragma pack(1)

/**
 * @brief U8 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_U8_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_u8_name_t name; /*!< Name of configuration parameter. */

    uint8_t value; /*!< Configuration value. */
} oscc_config_u8_s;

/**
 * @brief U16 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_U16_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_u16_name_t name; /*!< Name of configuration parameter. */

    uint16_t value; /*!< Configuration value. */
} oscc_config_u16_s;

/**
 * @brief U32 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_U32_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_u32_name_t name; /*!< Name of configuration parameter. */

    uint32_t value; /*!< Configuration value. */
} oscc_config_u32_s;

/**
 * @brief I8 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_I8_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_i8_name_t name; /*!< Name of configuration parameter. */

    int8_t value; /*!< Configuration value. */
} oscc_config_i8_s;

/**
 * @brief I16 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_I16_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_i16_name_t name; /*!< Name of configuration parameter. */

    int16_t value; /*!< Configuration value. */
} oscc_config_i16_s;

/**
 * @brief I32 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_I32_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_i32_name_t name; /*!< Name of configuration parameter. */

    int32_t value; /*!< Configuration value. */
} oscc_config_i32_s;

/**
 * @brief F32 configuration parameter.
 *
 * CAN frame ID: \ref OSCC_CONFIG_F32_CAN_ID
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    oscc_config_f32_name_t name; /*!< Name of configuration parameter. */

    float value; /*!< Configuration value. */
} oscc_config_f32_s;

/**
 * @brief Fault report message data.
 *
 * Message size (CAN frame DLC): \ref OSCC_FAULT_REPORT_CAN_DLC
 *
 */
typedef struct
{
    uint8_t magic[2]; /*!< Magic number identifying CAN frame as from OSCC.
                       *   Byte 0 should be \ref OSCC_MAGIC_BYTE_0.
                       *   Byte 1 should be \ref OSCC_MAGIC_BYTE_1. */

    uint32_t fault_origin_id; /*!< ID of the module that is sending out the fault. */

    uint8_t dtcs; /*!< DTC bitfield of the module that is sending out the fault. */

    uint8_t reserved; /*!< Reserved */
} oscc_fault_report_s;

#pragma pack(pop)


#endif /* _OSCC_GLOBAL_CAN_PROTOCOL_H_ */
