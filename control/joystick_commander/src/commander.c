/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */

/* Permission is hereby granted, free of charge, to any person */
/* obtaining a copy of this software and associated documentation */
/* files (the “Software”), to deal in the Software without */
/* restriction, including without limitation the rights to use, */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following */
/* conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR */
/* OTHER DEALINGS IN THE SOFTWARE. */
/************************************************************************/

/**
 * @file commander.c
 * @brief Commander Interface Source.
 *
 */




#include <canlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "joystick.h"
#include "messages.h"
#include "commander.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Throttle axis index.
 *
 */
#define JSTICK_AXIS_THROTTLE (5)


/**
 * @brief Brake axis index.
 *
 */
#define JSTICK_AXIS_BRAKE (2)


/**
 * @brief Steering axis index.
 *
 */
#define JSTICK_AXIS_STEER (3)


/**
 * @brief Enable controls button index.
 *
 */
#define JSTICK_BUTTON_ENABLE_CONTROLS (7)

/**
 * @brief Disable controls button index.
 *
 */
#define JSTICK_BUTTON_DISABLE_CONTROLS (6)


/**
 * @brief Convert degrees to radians.
 *
 */
#define m_radians(deg) ((deg)*(M_PI/180.0))


/**
 * @brief Convert radians to degrees.
 *
 */
#define m_degrees(rad) ((rad)*(180.0/M_PI))


/**
 * @brief Absolute value.
 *
 */
#define m_abs(x) ((x)>0?(x):-(x))




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************

//
static int get_brake_setpoint(
        joystick_device_s * const jstick,
        double * const brake );


//
static int get_throttle_setpoint(
        joystick_device_s * const jstick,
        double * const throttle );


//
static int get_steering_setpoint(
        joystick_device_s * const jstick,
        double * const angle );


//
static int get_disable_button(
        joystick_device_s * const jstick,
        unsigned int * const state );


//
static int get_enable_button(
        joystick_device_s * const jstick,
        unsigned int * const state );


//
static int publish_disable_brake_command(
        canHandle h,
        ps_ctrl_brake_command_msg * const msg );


//
static int publish_disable_throttle_command(
        canHandle h,
        ps_ctrl_throttle_command_msg * const msg );


//
static int publish_disable_steering_command(
        canHandle h,
        ps_ctrl_steering_command_msg * const msg );


//
static int publish_brake_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_brake_command_msg * const msg );


//
static int publish_throttle_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_throttle_command_msg * const msg );


//
static int publish_steering_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_steering_command_msg * const msg );




// *****************************************************
// static definitions
// *****************************************************

//
static int get_brake_setpoint(
        joystick_device_s * const jstick,
        double * const brake )
{
    int ret = NOERR;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_BRAKE,
            &axis_position );

    // if succeeded
    if( ret == NOERR )
    {       
        // set brake set point - scale to 0:max
        (*brake) = jstick_normalize_trigger_position(
                axis_position,
                0.0,
                MAX_BRAKE_PEDAL );
    }


    return ret;
}


//
static int get_throttle_setpoint(
        joystick_device_s * const jstick,
        double * const throttle )
{
    int ret = NOERR;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_THROTTLE,
            &axis_position );

    // if succeeded
    if( ret == NOERR )
    {
        // set throttle set point - scale to 0:max
        (*throttle) = jstick_normalize_trigger_position(
                axis_position,
                0.0,
                MAX_THROTTLE_PEDAL );
    }


    return ret;
}


//
static int get_steering_setpoint(
        joystick_device_s * const jstick,
        double * const angle )
{
    int ret = NOERR;
    int axis_position = 0;


    // read axis position
    ret = jstick_get_axis(
            jstick,
            JSTICK_AXIS_STEER,
            &axis_position );
    

    // if succeeded
    if( ret == NOERR )
    {
        // set steering wheel angle set point - scale to max:min
        // note that this is inverting the sign of the joystick axis
        (*angle) = jstick_normalize_axis_position(
                axis_position,
                MAX_STEERING_WHEEL_ANGLE,
                MIN_STEERING_WHEEL_ANGLE );
    }


    return ret;
}


//
static int get_disable_button(
        joystick_device_s * const jstick,
        unsigned int * const state )
{
    int ret = NOERR;
    unsigned int btn_state = JOYSTICK_BUTTON_STATE_NOT_PRESSED;


    // default state is disabled/not-pressed
    (*state) = 0;

    ret = jstick_get_button(
            jstick,
            JSTICK_BUTTON_DISABLE_CONTROLS,
            &btn_state );

    if( ret == NOERR )
    {
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            (*state) = 1;
        }
    }


    return ret;
}


//
static int get_enable_button(
        joystick_device_s * const jstick,
        unsigned int * const state )
{
    int ret = NOERR;
    unsigned int btn_state = JOYSTICK_BUTTON_STATE_NOT_PRESSED;


    // default state is disabled/not-pressed
    (*state) = 0;

    ret = jstick_get_button(
            jstick,
            JSTICK_BUTTON_ENABLE_CONTROLS,
            &btn_state );

    if( ret == NOERR )
    {
        if( btn_state == JOYSTICK_BUTTON_STATE_PRESSED )
        {
            (*state) = 1;
        }
    }


    return ret;
}


//
static int publish_disable_brake_command(
        canHandle h,
        ps_ctrl_brake_command_msg * const msg )
{
    int ret = NOERR;

    if( ret == NOERR )
    {
        msg->pedal_command = (uint16_t) 0.0;
        
        msg->enabled = 0;
    }
    
    printf( "break: %d %d\n", msg->enabled, msg->pedal_command );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_MSG_ID_BRAKE_COMMAND, (void *) msg, sizeof( ps_ctrl_brake_command_msg ), 0 );
    }


    return ret;
}


//
static int publish_disable_throttle_command(
        canHandle h,
        ps_ctrl_throttle_command_msg * const msg )
{
    int ret = NOERR;

    if( ret == NOERR )
    {
        msg->pedal_command = (uint16_t) 0.0;
        
        msg->enabled = 0;
    }
    
    printf( "throttle: %d %d\n", msg->enabled, msg->pedal_command );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_THROTTLE_COMMAND_ID, (void *) msg, sizeof( ps_ctrl_throttle_command_msg ), 0 );
    }


    return ret;
}


//
static int publish_disable_steering_command(
        canHandle h,
        ps_ctrl_steering_command_msg * const msg )
{
    int ret = NOERR;

    const float rate_degrees = (float) m_degrees(
            (float) STEERING_WHEEL_ANGLE_RATE_LIMIT );

    const float constrained_rate = (float) m_constrain(
            (float) (rate_degrees / (float) STEERING_COMMAND_MAX_VELOCITY_FACTOR),
            (float) STEERING_COMMAND_MAX_VELOCITY_MIN + 1.0f,
            (float) STEERING_COMMAND_MAX_VELOCITY_MAX );

    if( ret == NOERR )
    {
        msg->steering_wheel_angle_command = (int16_t) 0.0;
        
        msg->steering_wheel_max_velocity = (uint8_t) constrained_rate;
        
        msg->enabled = 0;
    }
    
    printf( "steering: %d %d %d\n", msg->enabled, msg->steering_wheel_angle_command, msg->steering_wheel_max_velocity );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_MSG_ID_STEERING_COMMAND, (void *) msg, sizeof( ps_ctrl_steering_command_msg ), 0 );
    }


    return ret;
}


//
static int publish_brake_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_brake_command_msg * const msg )
{
    int ret = NOERR;
    double brake_setpoint = 0.0;

    ret = get_brake_setpoint(
            jstick,
            &brake_setpoint );
    
    // Redundant, but better safe then sorry
    const float normalized_value = (float) m_constrain(
            (float) brake_setpoint,
            0.0f,
            1.0f );

    const float constrained_value = (float) m_constrain(
            (float) (normalized_value * (float) UINT16_MAX),
            (float) 0.0f,
            (float) UINT16_MAX );

    if( ret == NOERR )
    {
        msg->pedal_command = (uint16_t) constrained_value;
    }
    
    printf( "break: %d %d\n", msg->enabled, msg->pedal_command );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_MSG_ID_BRAKE_COMMAND, (void *) msg, sizeof( ps_ctrl_brake_command_msg ), 0 );
    }


    return ret;
}


//
static int publish_throttle_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_throttle_command_msg * const msg )
{
    int ret = NOERR;
    double throttle_setpoint = 0.0;
    double brake_setpoint = 0.0;


    ret = get_throttle_setpoint(
            jstick,
            &throttle_setpoint );

    // don't allow throttle if brakes are applied
    if( ret == NOERR )
    {
        ret = get_brake_setpoint(
            jstick,
            &brake_setpoint );

        if( brake_setpoint >= BRAKES_ENABLED_MIN )
        {
            throttle_setpoint = 0.0;
        }
    }
    
    // Redundant, but better safe then sorry
    const float normalized_value = (float) m_constrain(
            (float) throttle_setpoint,
            0.0f,
            MAX_THROTTLE_PEDAL );

    const float constrained_value = (float) m_constrain(
            (float) (normalized_value * (float) UINT16_MAX),
            (float) 0.0f,
            (float) UINT16_MAX );

    if( ret == NOERR )
    {
        msg->pedal_command = (uint16_t) constrained_value;
    }
    
    printf( "throttle: %d %d\n", msg->enabled, msg->pedal_command );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_THROTTLE_COMMAND_ID, (void *) msg, sizeof( ps_ctrl_throttle_command_msg ), 0 );
    }


    return ret;
}


//
static int publish_steering_command(
        canHandle h,
        joystick_device_s * const jstick,
        ps_ctrl_steering_command_msg * const msg )
{
    int ret = NOERR;
    double steering_setpoint = 0.0;


    ret = get_steering_setpoint(
            jstick,
            &steering_setpoint );
    
    const float angle_degrees = (float) m_degrees(
            (float) steering_setpoint );

    const float constrained_angle = (float) m_constrain(
            (float) (angle_degrees * (float) STEERING_COMMAND_ANGLE_FACTOR),
            (float) STEERING_COMMAND_ANGLE_MIN,
            (float) STEERING_COMMAND_ANGLE_MAX );

    const float rate_degrees = (float) m_degrees(
            (float) STEERING_WHEEL_ANGLE_RATE_LIMIT );

    const float constrained_rate = (float) m_constrain(
            (float) (rate_degrees / (float) STEERING_COMMAND_MAX_VELOCITY_FACTOR),
            (float) STEERING_COMMAND_MAX_VELOCITY_MIN + 1.0f,
            (float) STEERING_COMMAND_MAX_VELOCITY_MAX );

    if( ret == NOERR )
    {
        msg->steering_wheel_angle_command = (int16_t) constrained_angle;
        
        msg->steering_wheel_max_velocity = (uint8_t) constrained_rate;
    }
    
    printf( "steering: %d %d %d\n", msg->enabled, msg->steering_wheel_angle_command, msg->steering_wheel_max_velocity );

    if( ret == NOERR )
    {
        canWrite( h, PS_CTRL_MSG_ID_STEERING_COMMAND, (void *) msg, sizeof( ps_ctrl_steering_command_msg ), 0 );
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************

//
int commander_check_for_safe_joystick(
        commander_s * const commander )
{
    int ret = NOERR;
    double brake_setpoint = 0.0;
    double throttle_setpoint = 0.0;


    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // update joystick readings
        ret = jstick_update( &commander->joystick );

        // get brake set point
        ret |= get_brake_setpoint(
                &commander->joystick,
                &brake_setpoint );

        // get throttle set point
        ret |= get_throttle_setpoint(
                &commander->joystick,
                &throttle_setpoint );

        // handle DTC
        if( ret != NOERR )
        {
            // configuration error
            ret = ERROR;
        }

        // if succeeded
        if( ret == NOERR )
        {
            // if throttle not zero
            if( throttle_setpoint > 0.0 )
            {
                // invalidate
                ret = UNAVAILABLE;
            }

            // if brake not zero
            if( brake_setpoint > 0.0 )
            {
                // invalidate
                ret = UNAVAILABLE;
            }
        }
    }


    return ret;
}


//
int commander_is_valid(
        commander_s * const commander )
{
    int ret = NOERR;


    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        ret = messages_is_valid( &commander->messages );
    }


    return ret;
}


//
int commander_set_safe(
        commander_s * const commander )
{
    int ret = NOERR;


    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        ret = commander_is_valid( commander );

        if( ret == NOERR )
        {
            ret = messages_set_default_values(
                    &commander->messages );
        }
    }


    return ret;
}


//
int commander_enumerate_control_nodes(
        commander_s * const commander )
{
    int ret = NOERR;


    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // safe state
        ret = commander_set_safe( commander );

        // TODO: identify control nodes on CAN bus?
    }


    return ret;
}


//
int commander_disable_controls(
        commander_s * const commander )
{
    int ret = NOERR;


    printf( "Sending command to disable controls\n" );

    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // safe state
        ret = commander_set_safe( commander );

        // publish commands with disabled flag
        if( ret == NOERR )
        {
            ret = publish_disable_brake_command(
                    commander->canhandle,
                    &commander->messages.brake_cmd );
        }

        if( ret == NOERR )
        {
            ret = publish_disable_throttle_command(
                    commander->canhandle,
                    &commander->messages.throttle_cmd );
        }

        if( ret == NOERR )
        {
            ret = publish_disable_steering_command(
                    commander->canhandle,
                    &commander->messages.steering_cmd );
        }
    }


    return ret;
}


//
int commander_enable_controls(
        commander_s * const commander )
{
    int ret = NOERR;


    printf( "Enabling controls\n" );

    if( commander == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // safe state
        ret = commander_set_safe( commander );

        if( ret == NOERR )
        {
            commander->messages.brake_cmd.enabled = 1;
            
            commander->messages.steering_cmd.enabled = 1;
            
            commander->messages.throttle_cmd.enabled = 1;
        }
    }


    return ret;
}


//
int commander_update(
        commander_s * const commander )
{
    int ret = NOERR;
    unsigned int disable_button_pressed = 0;
    unsigned int enable_button_pressed = 0;


    if( commander == NULL )
    {
        ret = ERROR;
    }

    // safe state
    if( ret == ERROR )
    {
        ret = commander_set_safe( commander );
    }

    // update joystick
    if( ret == NOERR )
    {
        ret = jstick_update( &commander->joystick );
    }

    // get 'disable-controls' button state
    if( ret == NOERR )
    {
        ret = get_disable_button(
                &commander->joystick,
                &disable_button_pressed );
    }

    // get 'enable-controls' button state
    if( ret == NOERR )
    {
        ret = get_enable_button(
                &commander->joystick,
                &enable_button_pressed );
    }

    // only disable if both enable and disable buttons are pressed
    if( (enable_button_pressed != 0) && (disable_button_pressed != 0) )
    {
        enable_button_pressed = 0;
        disable_button_pressed = 1;
    }

    // send command if a enable/disable command
    if( (disable_button_pressed != 0) || (commander->driver_override == 1) )
    {
        ret = commander_disable_controls( commander );
        
        commander->driver_override = 0;
    }
    else if( enable_button_pressed != 0 )
    {
        ret = commander_enable_controls( commander );
    }
    else
    {
        // publish brake command continously
        if( ret == NOERR )
        {
            ret = publish_brake_command(
                    commander->canhandle,
                    &commander->joystick,
                    &commander->messages.brake_cmd );
        }

        // publish throttle command continously
        if( ret == NOERR )
        {
            ret = publish_throttle_command(
                    commander->canhandle,
                    &commander->joystick,
                    &commander->messages.throttle_cmd );
        }

        // publish steering command continously
        if( ret == NOERR )
        {
            ret = publish_steering_command(
                    commander->canhandle,
                    &commander->joystick,
                    &commander->messages.steering_cmd );
        }
    }

    return ret;
}
