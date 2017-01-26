/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */

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
 * @brief Joystick Interface Source
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




// *****************************************************
// Function:    jstick_init_state
// 
// Purpose:     Initialize the joystick state struct
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  jstick - pointer to the joystick to open
// 
// *****************************************************
int jstick_init_state( joystick_device_s * const jstick )
{
    int return_code = NOERR;
    
    if( jstick == NULL )
    {
        return_code = ERROR;
    }
    
    if( return_code == NOERR )
    {
        jstick->joystick_state.brake_setpoint_average = 0;

        jstick->joystick_state.throttle_setpoint_average = 0;

        jstick->joystick_state.steering_setpoint_average = 0;

        jstick->joystick_state.last_joystick_state_steering = 0;
    }
    
    return ( return_code );
}




// *****************************************************
// Function:    jstick_init_subsystem
// 
// Purpose:     Initialize the joystick subsystem
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  None
// 
// *****************************************************
int jstick_init_subsystem( )
{
    int return_code = NOERR;

    int init_result = SDL_Init( SDL_INIT_JOYSTICK );

    if ( init_result < 0 )
    {
        printf( "ERROR: SDL_Init - %s\n", SDL_GetError() );
        return_code = ERROR;
    }
    return ( return_code );
}


// *****************************************************
// Function:    jstick_release_subsystem
// 
// Purpose:     Release the joystick subsystem
// 
// Returns:     void
// 
// Parameters:  None
// 
// *****************************************************
void jstick_release_subsystem( )
{
    SDL_Quit();
}


// *****************************************************
// Function:    jstick_get_num_devices
// 
// Purpose:     Return the number of joystick devices resident on the system
// 
// Returns:     int - the number of devices or ERROR
// 
// Parameters:  None
// 
// *****************************************************
int jstick_get_num_devices( )
{
    int num_joysticks = SDL_NumJoysticks();

    if ( num_joysticks < 0 )
    {
        printf( "ERROR: SDL_NumJoysticks - %s\n", SDL_GetError() );
        num_joysticks = ERROR;
    }

    return ( num_joysticks );
}


// *****************************************************
// Function:    jstick_get_guid_at_index
// 
// Purpose:     Return the Globally Unique ID (GUID) for the requested joystick
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  device_index - index to the requested device
//              guid - pointer to the guid to fill out
// 
// *****************************************************
int jstick_get_guid_at_index( const unsigned long device_index,
                              joystick_guid_s * const guid )
{
    int return_code = ERROR;

    if ( guid != NULL )
    {
        return_code = NOERR;

        const SDL_JoystickGUID m_guid =
            SDL_JoystickGetDeviceGUID( (int) device_index );

        memcpy( guid->data, m_guid.data, sizeof(m_guid.data) );

        memset( guid->ascii_string, 0, sizeof(guid->ascii_string) );

        SDL_JoystickGetGUIDString( m_guid,
                                   guid->ascii_string,
                                   sizeof(guid->ascii_string) );
    }

    return ( return_code );
}


// *****************************************************
// Function:    jstick_open
// 
// Purpose:     Open the requested joystick for use
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  device_index - index to the requested device
//              jstick - pointer to the joystick to open
// 
// *****************************************************
int jstick_open( const unsigned long device_index,
                 joystick_device_s * const jstick )
{
    int return_code = ERROR;

    if( jstick != NULL )
    {
        jstick->handle = (void*) SDL_JoystickOpen( (int) device_index );

        if( jstick->handle == JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            printf( "ERROR: SDL_JoystickOpen - %s\n", SDL_GetError() );
        }
        else
        {
            return_code = NOERR;

            const SDL_JoystickGUID m_guid =
                SDL_JoystickGetGUID( jstick->handle );

            memcpy( jstick->guid.data, m_guid.data, sizeof( m_guid.data ) );

            memset( jstick->guid.ascii_string,
                    0,
                    sizeof( jstick->guid.ascii_string ) );

            SDL_JoystickGetGUIDString( m_guid,
                                       jstick->guid.ascii_string,
                                       sizeof( jstick->guid.ascii_string ) );
        }
    }

    return ( return_code );
}


// *****************************************************
// Function:    jstick_close
// 
// Purpose:     Close the requested joystick for use
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  device_index - index to the requested device
//              jstick - pointer to the joystick to close
// 
// *****************************************************
void jstick_close( joystick_device_s * const jstick )
{
    if ( jstick != NULL )
    {
        if ( jstick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            if ( SDL_JoystickGetAttached( jstick->handle ) == SDL_TRUE )
            {
                SDL_JoystickClose( jstick->handle );
            }

            jstick->handle = JOYSTICK_DEVICE_HANDLE_INVALID;
        }
    }
}


// *****************************************************
// Function:    jstick_update
// 
// Purpose:     Update the requested joystick for use
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  device_index - index to the requested device
//              jstick - pointer to the joystick to update
// 
// *****************************************************
int jstick_update( joystick_device_s * const jstick )
{
    int return_code = ERROR;

    if ( jstick != NULL )
    {
        if ( jstick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            SDL_JoystickUpdate();

            if ( SDL_JoystickGetAttached( jstick->handle ) == SDL_FALSE )
            {
                printf( "SDL_JoystickGetAttached - device not attached\n" );
            }
            else
            {
                return_code = NOERR;
            }
        }
    }

    return return_code;
}


// *****************************************************
// Function:    jstick_get_axis
// 
// Purpose:     Get the axis index for the requested joystick
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  jstick - pointer to the joystick to update
//              axis_index - index to the axis to use
//              position - pointer to the position to update
// 
// *****************************************************
int jstick_get_axis( joystick_device_s * const jstick,
                     const unsigned long axis_index,
                     int * const position )
{
    int return_code = ERROR;

    if ( ( jstick != NULL ) && ( position != NULL ) )
    {
        return_code = NOERR;

        const Sint16 pos = SDL_JoystickGetAxis( jstick->handle,
                                                (int) axis_index );

        (*position) = (int) pos;
    }

    return return_code;
}


// *****************************************************
// Function:    jstick_get_button
// 
// Purpose:     Get which button was pressed for the requested joystick
// 
// Returns:     int - ERROR or NOERROR
// 
// Parameters:  jstick - pointer to the joystick to update
//              button_index - index to the button to use
//              button_state - pointer to the button state to update
// 
// *****************************************************
int jstick_get_button( joystick_device_s * const jstick,
                       const unsigned long button_index,
                       unsigned int * const button_state )
{
    int return_code = ERROR;

    if ( ( jstick != NULL ) && ( button_state != NULL ) )
    {
        return_code = NOERR;

        const Uint8 m_state = SDL_JoystickGetButton( jstick->handle,
                                                     (int) button_index );

        ( *button_state ) = JOYSTICK_BUTTON_STATE_NOT_PRESSED;

        if ( m_state == 1 )
        {
            ( *button_state ) = JOYSTICK_BUTTON_STATE_PRESSED;
            ( void ) usleep( BUTTON_PRESSED_DELAY );
        }
    }

    return return_code;
}


double jstick_calc_log_range( double a1,
                              double a2,
                              double b1,
                              double b2,
                              double position )
{
    const double x1 = 1.0; // cannot include zero in range
    const double y1 = 1.0; // cannot include zero in range
    const double x2 = a2 - a1 + 1.0; // add one to make sure the range doesn't include zero
    const double y2 = b2 - b1 + 1.0; // add one to make sure the range doesn't include zero
    
    const double b = log( y1 / y2 ) / ( x1 - x2 );
    const double a = y1 / exp( b * x1 );
    
    double result = a * exp( b * (position - a1 + 1.0) );
    return result + b1 - 1.0; // normalize back to correct range
}

// *****************************************************
// Function:    jstick_normalize_axis_position
// 
// Purpose:     Convert the integer current joystick input position into a
//              scaled value for the variable that the joystick input
//              represents
// 
// Returns:     double - the normalized axis position
// 
// Parameters:  position - current input position on the joystick
//              range_min - minimum scaled value for the variable
//              range_max - maximum scaled value for the variable
// 
// *****************************************************
double jstick_normalize_axis_position( const int position,
                                       const double range_min,
                                       const double range_max )
{
    const double s = (double) position;
    
    double a1, a2, b1, b2;
    
    if( position < 0 )
    {
        a1 = 0;
        a2 = JOYSTICK_AXIS_POSITION_MIN;
        b1 = 0;
        b2 = range_max;
    }
    else
    {
        a1 = JOYSTICK_AXIS_POSITION_MAX;
        a2 = 0;
        b1 = range_min;
        b2 = 0;
    }
    
    // map value s in the range of a1 and a2, to t(return) in the range b1 and b2, exponential
    return jstick_calc_log_range( a1, a2, b1, b2, s );
}


// *****************************************************
// Function:    jstick_normalize_trigger_position
// 
// Purpose:     Convert the integer current joystick trigger position
//              to a scaled value
// 
// Returns:     double - the normalized trigger position
// 
// Parameters:  position - current trigger position on the joystick
//              range_min - minimum scaled value for the variable
//              range_max - maximum scaled value for the variable
// 
// *****************************************************
double jstick_normalize_trigger_position( const int position,
                                          const double range_min,
                                          const double range_max )
{
    const double s = (double) position;
    const double a1 = (double) JOYSTICK_AXIS_POSITION_MIN;
    const double a2 = (double) JOYSTICK_AXIS_POSITION_MAX;
    
    const double b1 = range_min;
    const double b2 = range_max;
    
    return jstick_calc_log_range( a1, a2, b1, b2, s );
}


// *****************************************************
// Function:    jstick_calc_exponential_average 
// 
// Purpose:     Calculate an exponential average based on previous values.
// 
// Returns:     double - the exponentially averaged result.
// 
// Parameters:  average - pointer to previous result of exponential averaging
//              setpoint - new setpoint to incorperate into average
//              factor - factor of exponential average
// 
// *****************************************************
double jstick_calc_exponential_average( double * const average,
                                        const double setpoint,
                                        const double factor )
{
    (*average) = setpoint * factor + ( 1 - factor ) * (*average);
    
    return (*average);
}
