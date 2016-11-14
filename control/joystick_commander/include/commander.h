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
 * @file commander.h
 * @brief Commander Interface.
 *
 */


#ifndef COMMANDER_H
#define COMMANDER_H




#include <math.h>

#include "joystick.h"
#include "messages.h"




/**
 * @brief Maximum allowed throttle pedal position value. [normalized]
 *
 */
#define MAX_THROTTLE_PEDAL (0.3)


/**
 * @brief Maximum allowed brake pedal position value. [normalized]
 *
 */
#define MAX_BRAKE_PEDAL (0.8)


/**
 * @brief Minimum brake value to be considered enabled. [normalized]
 *
 * Throttle is disabled when brake value is greate than this value.
 *
 */
#define BRAKES_ENABLED_MIN (0.05)


/**
 * @brief Minimum allowed steering wheel angle value. [radians]
 *
 * Negative value means turning to the right.
 *
 */
#define MIN_STEERING_WHEEL_ANGLE (-M_PI * 2.0)


/**
 * @brief Maximum allowed steering wheel angle value. [radians]
 *
 * Positive value means turning to the left.
 *
 */
#define MAX_STEERING_WHEEL_ANGLE (M_PI * 2.0)


/**
 * @brief Maximum allowed absolute steering wheel angle rate value. [radians/second]
 *
 */
#define STEERING_WHEEL_ANGLE_RATE_LIMIT (M_PI_2)


/**
 * @brief Steering command angle minimum valid value. [int16_t]
 *
 */
#define STEERING_COMMAND_ANGLE_MIN (-4700)


/**
 * @brief Steering command angle maximum valid value. [int16_t]
 *
 */
#define STEERING_COMMAND_ANGLE_MAX (4700)


/**
 * @brief Steering command angle scale factor.
 *
 */
#define STEERING_COMMAND_ANGLE_FACTOR (10)


/**
 * @brief Steering command steering wheel velocity minimum valid value. [uint8_t]
 *
 */
#define STEERING_COMMAND_MAX_VELOCITY_MIN (0)


/**
 * @brief Steering command steering wheel velocity maximum valid value. [uint8_t]
 *
 */
#define STEERING_COMMAND_MAX_VELOCITY_MAX (254)


/**
 * @brief Steering command steering wheel velocity scale factor.
 *
 */
#define STEERING_COMMAND_MAX_VELOCITY_FACTOR (2)




/**
 * @brief Commander node data.
 *
 * Serves as a top-level container for the application's data structures.
 *
 */
typedef struct
{
    //
    //
    joystick_device_s joystick; /*!< Joystick handle. */
    //
    //
    messages_s messages; /*!< PolySync messages. */
    //
    //
    unsigned long long last_commander_update; /*!< Last commander update timestamp. [microseconds] */
    //
    //
    canHandle canhandle; /*!< Handle to CAN interface. */
} commander_s;




/**
 * @brief Wait for joystick throttle/brake values to be zero.
 *
 * @param [in] commander A pointer to \ref commander_s which specifies the joystick configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if joystick values safe.
 * \li \ref DTC_USAGE if arguments are invalid.
 * \li \ref DTC_CONFIG if configuration invalid.
 * \li \ref DTC_UNAVAILABLE if joystick values are not safe.
 *
 */
int commander_check_for_safe_joystick(
        commander_s * const commander );


//
int commander_is_valid(
        commander_s * const commander );


/**
 * @brief Set control command messages to their safe state.
 *
 * @param [in] commander A pointer to \ref commander_s which receives the safe state configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int commander_set_safe(
        commander_s * const commander );


//
int commander_enumerate_control_nodes(
        commander_s * const commander );


//
int commander_disable_controls(
        commander_s * const commander );


//
int commander_enable_controls(
        commander_s * const commander );


//
int commander_update(
        commander_s * const commander );




#endif	/* COMMANDER_H */
