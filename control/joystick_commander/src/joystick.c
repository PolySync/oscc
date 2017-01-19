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
 * @file joystick.c
 * @brief Joystick Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>

#include "macros.h"
#include "joystick.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Button press debounce delay. [microseconds]
 *
 */
#define BUTTON_PRESSED_DELAY (5000)



// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static declarations
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************




// *****************************************************
// public definitions
// *****************************************************

//
int jstick_init_subsystem( void )
{
    // init joystick subsystem
    int ret = SDL_Init( SDL_INIT_JOYSTICK );

    // error check
    if( ret < 0 )
    {
        printf(
                "ERROR: SDL_Init - %s\n",
                SDL_GetError() );
        return ERROR;
    }
    else
    {
        return NOERR;
    }
}


//
void jstick_release_subsystem( void )
{
    // release
    SDL_Quit();
}


//
int jstick_get_num_devices( void )
{
    // local vars
    int ret = NOERR;


    // get number of joysticks on the system
    ret = SDL_NumJoysticks();

    // error check
    if( ret < 0 )
    {
        printf(
                "ERROR: SDL_NumJoysticks - %s\n",
                SDL_GetError() );
        return ERROR;
    }


    // return count
    return ret;
}


//
int jstick_get_guid_at_index(
        const unsigned long device_index,
        joystick_guid_s * const guid )
{
    if( guid == NULL )
    {
        return ERROR;
    }


    // get GUID
    const SDL_JoystickGUID m_guid =
            SDL_JoystickGetDeviceGUID( (int) device_index );

    // copy
    memcpy( guid->data, m_guid.data, sizeof(m_guid.data) );

    // get string representation
    memset( guid->ascii_string, 0, sizeof(guid->ascii_string) );
    SDL_JoystickGetGUIDString(
            m_guid,
            guid->ascii_string,
            sizeof(guid->ascii_string) );


    return NOERR;
}


//
int jstick_open(
        const unsigned long device_index,
        joystick_device_s * const jstick )
{
    if( jstick == NULL )
    {
        return ERROR;
    }


    // open joystick at index
    jstick->handle = (void*) SDL_JoystickOpen( (int) device_index );

    // error check
    if( jstick->handle == JOYSTICK_DEVICE_HANDLE_INVALID )
    {
        printf(
                "ERROR: SDL_JoystickOpen - %s\n",
                SDL_GetError() );
        return ERROR;
    }

    // get GUID
    const SDL_JoystickGUID m_guid =
            SDL_JoystickGetGUID( jstick->handle );

    // copy
    memcpy( jstick->guid.data, m_guid.data, sizeof(m_guid.data) );

    // get string representation
    memset( jstick->guid.ascii_string, 0, sizeof(jstick->guid.ascii_string) );
    SDL_JoystickGetGUIDString(
            m_guid,
            jstick->guid.ascii_string,
            sizeof(jstick->guid.ascii_string) );


    return NOERR;
}


//
void jstick_close(
    joystick_device_s * const jstick )
{
    if( jstick == NULL )
    {
        return;
    }


    // if handle valid
    if( jstick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
    {
        // if attached
        if( SDL_JoystickGetAttached( jstick->handle ) == SDL_TRUE )
        {
            // close
            SDL_JoystickClose( jstick->handle );
        }

        // invalidate
        jstick->handle = JOYSTICK_DEVICE_HANDLE_INVALID;
    }
}


//
int jstick_update(
    joystick_device_s * const jstick )
{
    if( jstick == NULL )
    {
        return ERROR;
    }

    // local vars
    int ret = NOERR;


    // if handle valid
    if( jstick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
    {
        // update
        SDL_JoystickUpdate();

        // check if attached
        if( SDL_JoystickGetAttached(jstick->handle) == SDL_FALSE )
        {
            printf( "SDL_JoystickGetAttached - device not attached\n" );

            // invalid handle
            ret = ERROR;
        }
    }
    else
    {
        // invalid handle
        ret = ERROR;
    }


    return ret;
}


//
int jstick_get_axis(
        joystick_device_s * const jstick,
        const unsigned long axis_index,
        int * const position )
{
    if( (jstick == NULL) || (position == NULL) )
    {
        return ERROR;
    }


    // zero
    (*position) = 0;

    // get axis value
    const Sint16 pos = SDL_JoystickGetAxis(
            jstick->handle,
            (int) axis_index );

    // convert
    (*position) = (int) pos;


    return NOERR;
}


//
int jstick_get_button(
        joystick_device_s * const jstick,
        const unsigned long button_index,
        unsigned int * const state )
{
    if( (jstick == NULL) || (state == NULL) )
    {
        return ERROR;
    }


    // zero
    (*state) = JOYSTICK_BUTTON_STATE_NOT_PRESSED;

    // get button state
    const Uint8 m_state = SDL_JoystickGetButton(
            jstick->handle,
            (int) button_index );

    // convert
    if( m_state == 1 )
    {
        (*state) = JOYSTICK_BUTTON_STATE_PRESSED;
        (void) usleep( BUTTON_PRESSED_DELAY );
    }
    else
    {
        (*state) = JOYSTICK_BUTTON_STATE_NOT_PRESSED;
    }


    return NOERR;
}


//
double jstick_normalize_axis_position(
        const int position,
        const double range_min,
        const double range_max )
{
    const double s = (double) position;
    
    double a1;
    double a2;
    double b1;
    double b2;
    
    if( position < 0 )
    {
        a1 = 0;
        a2 = JOYSTICK_AXIS_POSITION_MIN;
        b1 = 0;
        b2 = range_min;
    }
    else
    {
        a1 = JOYSTICK_AXIS_POSITION_MAX;
        a2 = 0;
        b1 = range_max;
        b2 = 0;
    }
    
    const double x1 = 1.0; // cannot include zero in range
    const double y1 = 1.0; // cannot include zero in range
    const double x2 = a2 - a1 + 1.0; // add one to make sure the range doesn't include zero
    const double y2 = b2 - b1 + 1.0; // add one to make sure the range doesn't include zero
    
    const double b = log( y1 / y2 )/( x1 - x2 );
    const double a = y1 / exp( b * x1 );
    
    double result = a * exp( b * (position - a1 + 1.0) );
    result = result + b1 - 1.0; // normalize back to correct range
    
    // map value s in the range of a1 and a2, to t(return) in the range b1 and b2, exponential
    return result;
}


//
double jstick_normalize_trigger_position(
        const int position,
        const double range_min,
        const double range_max )
{
    const double s = (double) position;
    const double a1 = (double) JOYSTICK_AXIS_POSITION_MIN;
    const double a2 = (double) JOYSTICK_AXIS_POSITION_MAX;
    
    const double b1 = range_min;
    const double b2 = range_max;
    
    const double x1 = 1.0; // cannot include zero in range
    const double y1 = 1.0; // cannot include zero in range
    const double x2 = a2 - a1 + 1.0; // add one to make sure the range doesn't include zero
    const double y2 = b2 - b1 + 1.0; // add one to make sure the range doesn't include zero
    
    const double b = log( y1 / y2 )/( x1 - x2 );
    const double a = y1 / exp( b * x1 );
    
    double result = a * exp( b * (position - a1 + 1.0) );
    result = result + b1 - 1.0; // normalize back to correct range
    
    // map value s in the range of a1 and a2, to t(return) in the range b1 and b2, exponential
    return result;
}
