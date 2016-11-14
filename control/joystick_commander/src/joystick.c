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
 * @file joystick.c
 * @brief Joystick Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
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
    const double a1 = (double) JOYSTICK_AXIS_POSITION_MIN;
    const double a2 = (double) JOYSTICK_AXIS_POSITION_MAX;
    const double b1 = range_min;
    const double b2 = range_max;


    // map value s in the range of a1 and a2, to t(return) in the range b1 and b2, linear
    return b1 + (s-a1) * (b2-b1) / (a2-a1);
}
