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

/**
 * @brief Invalid \ref joystick_device_s.handle value
 *
 */
#define JOYSTICK_DEVICE_HANDLE_INVALID ( NULL )

/**
 * @brief Joystick Identifier Data
 *
 */
#define JOYSTICK_ID_DATA_SIZE ( 16 )

/**
 * @brief Joystick Description String
 *
 */
#define JOYSTICK_ID_STRING_SIZE ( 64 )



// *****************************************************
// type definitions
// *****************************************************


/**
 * @brief Joystick GUID
 *
 * Implementation-dependent GUID
 *
 */
typedef struct
{
    unsigned char data[ JOYSTICK_ID_DATA_SIZE ];
    char ascii_string[ JOYSTICK_ID_STRING_SIZE ];

} joystick_guid_s;


/**
 * @brief Joystick device
 *
 */
typedef struct
{
    void *handle;

    joystick_guid_s* guid;

} joystick_device_data_s;


// *****************************************************
// static global data
// *****************************************************


static joystick_guid_s joystick_guid;
static joystick_device_data_s joystick_data = { NULL, &joystick_guid };
static joystick_device_data_s* joystick = NULL;


// *****************************************************
// static definitions
// *****************************************************


// *****************************************************
// Function:    joystick_init_subsystem
//
// Purpose:     Initialize the joystick subsystem
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  None
//
// *****************************************************
static int joystick_init_subsystem( )
{
    int return_code = ERROR;

    if ( joystick == NULL )
    {
        int init_result = SDL_Init( SDL_INIT_JOYSTICK );

        return_code = NOERR;

        if ( init_result < 0 )
        {
            printf( "ERROR: SDL_Init - %s\n", SDL_GetError() );
            return_code = ERROR;
        }
    }
    return ( return_code );
}


// *****************************************************
// Function:    joystick_get_guid_at_index
//
// Purpose:     Return the Globally Unique ID (GUID) for the requested joystick
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  device_index - index to the requested device
//              guid - pointer to the guid to fill out
//
// *****************************************************
static int joystick_get_guid_at_index( unsigned long device_index )
{
    int return_code = ERROR;

    if ( joystick != NULL )
    {
        return_code = NOERR;

        const SDL_JoystickGUID m_guid =
            SDL_JoystickGetDeviceGUID( (int) device_index );

        memcpy( joystick_guid.data, m_guid.data, sizeof( m_guid.data ) );

        memset( joystick_guid.ascii_string, 0,
                sizeof( joystick_guid.ascii_string ) );

        SDL_JoystickGetGUIDString( m_guid,
                                   joystick_guid.ascii_string,
                                   sizeof( joystick_guid.ascii_string ) );
    }
    return ( return_code );
}


// *****************************************************
// Function:    joystick_get_num_devices
//
// Purpose:     Return the number of joystick devices resident on the system
//
// Returns:     int - the number of devices or ERROR
//
// Parameters:  None
//
// *****************************************************
static int joystick_get_num_devices( )
{
    int num_joysticks = ERROR;

    if ( joystick != NULL )
    {
        num_joysticks = SDL_NumJoysticks();

        if ( num_joysticks < 0 )
        {
            printf( "ERROR: SDL_NumJoysticks - %s\n", SDL_GetError() );
            num_joysticks = ERROR;
        }
    }
    return ( num_joysticks );
}


// *****************************************************
// Function:    joystick_curve_fit
//
// Purpose:     Calculate the logarithmic range for the joystick position
//
//              Create the formula:
//              output = ( output_range / ( input_range )^2 ) * ( input )^2
//
//              Which is equal to:
//              output = output_range * ( input / input_range )^2
//
// Returns:     double - the curve fit value
//
// Parameters:  input_min - input range min
//              input_max - input range max
//              output_min - output range min
//              output_max - output range max
//              position - current joystick position
//
// *****************************************************
static double joystick_curve_fit( double input_min,
                                  double input_max,
                                  double output_min,
                                  double output_max,
                                  double position )
{
    const double input_range = input_max - input_min;
    const double output_range = output_max - output_min;

    double input = position - input_min;
    double output = 0.0;

    input /= input_range;   // input / input_range
    input *= input;         // ( input / input_range )^2

    // output = output_range * ( input / input_range )^2
    output = output_range * input;

    output += output_min;   // normalize to output range

    return ( output );
}



// *****************************************************
// public definitions
// *****************************************************



// *****************************************************
// Function:    joystick_init_subsystem
//
// Purpose:     Initialize the joystick subsystem
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  None
//
// *****************************************************
int joystick_init( )
{
    int return_code = NOERR;

    return_code = joystick_init_subsystem();

    if ( return_code == ERROR )
    {
        printf("init subsystem error\n");
    }
    else
    {
        joystick = &joystick_data;
        joystick->handle = JOYSTICK_DEVICE_HANDLE_INVALID;

        const int num_joysticks = joystick_get_num_devices();

        if ( num_joysticks > 0 )
        {
            unsigned long device_index = 0;

            return_code = joystick_get_guid_at_index( device_index );

            if ( return_code == NOERR )
            {
                printf( "Found %d devices -- connecting to device at system index %lu - GUID: %s\n",
                        num_joysticks,
                        device_index,
                        joystick_guid.ascii_string );

                return_code = joystick_open( device_index );
            }
        }
        else
        {
            printf( "No joystick/devices available on the host\n" );
        }
    }

}


// *****************************************************
// Function:    joystick_open
//
// Purpose:     Open the requested joystick for use
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  device_index - index to the requested device
//
// *****************************************************
int joystick_open( unsigned long device_index )
{
    int return_code = ERROR;

    if ( joystick != NULL )
    {
        joystick->handle = (void*) SDL_JoystickOpen( (int) device_index );

        if ( joystick->handle == JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            printf( "ERROR: SDL_JoystickOpen - %s\n", SDL_GetError() );
        }
        else
        {
            return_code = NOERR;

            const SDL_JoystickGUID m_guid =
                SDL_JoystickGetGUID( joystick->handle );

            memcpy( joystick_guid.data, m_guid.data, sizeof( m_guid.data ) );

            memset( joystick_guid.ascii_string,
                    0,
                    sizeof( joystick_guid.ascii_string ) );

            SDL_JoystickGetGUIDString( m_guid,
                                       joystick_guid.ascii_string,
                                       sizeof( joystick_guid.ascii_string ) );
        }
    }
    return ( return_code );
}


// *****************************************************
// Function:    jstick_close
//
// Purpose:     Close the joystick for use
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void joystick_close( )
{
    if ( joystick != NULL )
    {
        if ( joystick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            if ( SDL_JoystickGetAttached( joystick->handle ) == SDL_TRUE )
            {
                SDL_JoystickClose( joystick->handle );
            }

            joystick->handle = JOYSTICK_DEVICE_HANDLE_INVALID;
        }
        joystick = NULL;
    }
    // Release the joystick subsystem
    SDL_Quit();
}


// *****************************************************
// Function:    joystick_update
//
// Purpose:     Update the requested joystick for use
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  device_index - index to the requested device
//
// *****************************************************
int joystick_update( )
{
    int return_code = ERROR;

    if ( joystick != NULL )
    {
        if ( joystick->handle != JOYSTICK_DEVICE_HANDLE_INVALID )
        {
            SDL_JoystickUpdate();

            if ( SDL_JoystickGetAttached( joystick->handle ) == SDL_FALSE )
            {
                printf( "SDL_JoystickGetAttached - device not attached\n" );
            }
            else
            {
                return_code = NOERR;
            }
        }
    }
    return ( return_code );
}


// *****************************************************
// Function:    joystick_get_axis
//
// Purpose:     Get the axis index
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  axis_index - index to the axis to use
//              position - pointer to the position to update
//
// *****************************************************
int joystick_get_axis( unsigned long axis_index, int * const position )
{
    int return_code = ERROR;

    if ( ( joystick  != NULL ) && ( position != NULL ) )
    {
        return_code = NOERR;

        const Sint16 pos = SDL_JoystickGetAxis( joystick->handle,
                                                (int) axis_index );
        ( *position ) = (int) pos;
    }

    return return_code;
}


// *****************************************************
// Function:    joystick_get_button
//
// Purpose:     Get which button was pressed for the requested joystick
//
// Returns:     int - ERROR or NOERROR
//
// Parameters:  button_index - index to the button to use
//              button_state - pointer to the button state to update
//
// *****************************************************
int joystick_get_button( unsigned long button_index,
                         unsigned int * const button_state )
{
    int return_code = ERROR;

    if ( ( joystick  != NULL ) && ( button_state != NULL ) )
    {
        return_code = NOERR;

        const Uint8 m_state = SDL_JoystickGetButton( joystick->handle,
                                                     (int) button_index );

        if ( m_state == 1 )
        {
            ( *button_state ) = JOYSTICK_BUTTON_STATE_PRESSED;
            ( void ) usleep( BUTTON_PRESSED_DELAY );
        }
        else
        {
            ( *button_state ) = JOYSTICK_BUTTON_STATE_NOT_PRESSED;
        }
    }

    return return_code;
}


// *****************************************************
// Function:    joystick_normalize_axis_position
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
double joystick_normalize_axis_position( const int position,
                                         const double range_min,
                                         const double range_max )
{
    const double input_min = 0.0;
    const double output_min = 0.0;

    double input_max = ( double )JOYSTICK_AXIS_POSITION_MAX;
    double output_max = range_min;

    if ( position < 0 )
    {
        input_max = ( double )JOYSTICK_AXIS_POSITION_MIN;
        output_max = range_max;
    }

    const double output = joystick_curve_fit( input_min,
                                              input_max,
                                              output_min,
                                              output_max,
                                              ( double )position );

    return ( output );
}


// *****************************************************
// Function:    joystick_normalize_trigger_position
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
double joystick_normalize_trigger_position( const int position,
                                            const double range_min,
                                            const double range_max )
{
    const double output = joystick_curve_fit(
        ( double )JOYSTICK_AXIS_POSITION_MIN,
        ( double )JOYSTICK_AXIS_POSITION_MAX,
        range_min,
        range_max,
        ( double )position );

    return ( output );
}


