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
 * @file commander.c
 * @brief Commander Interface Source
 *
 */




#include <canlib.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>

#include "macros.h"
#include "joystick.h"
#include "oscc_interface.h"


// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief Joystick axis indices
 *
 */
#define JOYSTICK_AXIS_THROTTLE (5)
#define JOYSTICK_AXIS_BRAKE (2)
#define JOYSTICK_AXIS_STEER (3)


/**
 * @brief Joystick button indices
 *
 */
#define JOYSTICK_BUTTON_ENABLE_CONTROLS (7)
#define JOYSTICK_BUTTON_DISABLE_CONTROLS (6)

/**
 * @brief Throttle pedal position values [normalized]
 *
 */
#define MIN_THROTTLE_PEDAL (0.0)
#define MAX_THROTTLE_PEDAL (0.3)


/**
 * @brief Brake pedal position values [normalized]
 *
 */
#define MIN_BRAKE_PEDAL (0.0)
#define MAX_BRAKE_PEDAL (0.8)


/**
 * @brief Minimum brake value to be considered enabled [normalized]
 *
 * Throttle is disabled when brake value is greater than this value
 *
 */
#define BRAKES_ENABLED_MIN (0.05)


/**
 * @brief Steering wheel angle values [radians]
 *
 * Negative value means turning to the right
 *
 */
#define MIN_STEERING_WHEEL_ANGLE (-M_PI * 2.0)
#define MAX_STEERING_WHEEL_ANGLE (M_PI * 2.0)


/**
 * @brief Steering command angles [int16_t]
 *
 */
#define STEERING_COMMAND_ANGLE_MIN (-4700)
#define STEERING_COMMAND_ANGLE_MAX (4700)


/**
 * @brief Steering command angle scale factor
 *
 */
#define STEERING_COMMAND_ANGLE_FACTOR ( 10.0 )


/**
 * @brief Steering command steering wheel velocities [uint8_t]
 *
 */
#define STEERING_COMMAND_MAX_VELOCITY_MIN (20)
#define STEERING_COMMAND_MAX_VELOCITY_MAX (254)


/**
 * @brief Steering command steering wheel velocity scale factor
 *
 * This factor can be increased to provide smoother, but
 * slightly less responsive, steering control. It is recommended
 * to smooth at the higher level, with this factor, before
 * trying to smooth at the lower level
 *
 */
#define STEERING_COMMAND_MAX_VELOCITY_FACTOR (0.25)


/**
 * @brief Exponential filter factors
 *
 */
#define BRAKES_FILTER_FACTOR (0.2)
#define THROTTLE_FILTER_FACTOR (0.2)
#define STEERING_FILTER_FACTOR (0.1)

/**
 * @brief joystick delay interval [microseconds]
 *
 * Defines the delay to wait for the joystick to update
 *
 * 50,000 us == 50 ms == 20 Hertz
 *
 */
#define JOYSTICK_DELAY_INTERVAL (50000)


/**
 * @brief Convert radians to degrees
 *
 */
#define m_degrees(rad) ( ( rad ) * ( 180.0 / M_PI ) )


// *****************************************************
// Local Type definitions
// *****************************************************

/**
 * @brief Commander setpoint
 *
 * The commander setpoint is a structure that contains all the
 * relevant information to retrieve data from an external source
 * and range check it for validity.  The range-limits for this
 * instance represent the values that are typically available
 * from a joystick
 *
 */
struct commander_setpoint_s
{
    double setpoint;

    const unsigned long axis;

    const double min_position;

    const double max_position;
};


// *****************************************************
// static global data
// *****************************************************

#define COMMANDER_ENABLED ( 1 )
#define COMMANDER_DISABLED ( 0 )

static int commander_enabled = COMMANDER_DISABLED;

/**
 * @brief Setpoint Data
 *
 * Static definitions for brake, steering and throttle setpoints
 *
 */
static struct commander_setpoint_s brake_setpoint =
    { 0.0, JOYSTICK_AXIS_BRAKE, MIN_BRAKE_PEDAL, MAX_BRAKE_PEDAL };

static struct commander_setpoint_s throttle_setpoint =
    { 0.0, JOYSTICK_AXIS_THROTTLE, MIN_THROTTLE_PEDAL, MAX_THROTTLE_PEDAL };

static struct commander_setpoint_s steering_setpoint =
    { 0.0, JOYSTICK_AXIS_STEER, MIN_STEERING_WHEEL_ANGLE, MAX_STEERING_WHEEL_ANGLE };


// *****************************************************
// static definitions
// *****************************************************


// *****************************************************
// Function:    get_setpoint
//
// Purpose:     Retrieve the data from the joystick based on which axis is
//              selected and normalize that value along the scale that is
//              provided
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  setpoint - the setpoint structure containing the range limits
//                         the joystick axis, and the storage location for the
//                         requested value
//
// *****************************************************
static int get_setpoint( struct commander_setpoint_s* setpoint )
{
    int return_code = ERROR;

    if ( setpoint != NULL )
    {
        int axis_position = 0;

        return_code = joystick_get_axis( setpoint->axis, &axis_position );

        if ( return_code == NOERR )
        {
            if ( setpoint->axis == JOYSTICK_AXIS_STEER )
            {
                setpoint->setpoint =
                    joystick_normalize_axis_position( axis_position,
                                                      setpoint->min_position,
                                                      setpoint->max_position );
            }
            else
            {
                setpoint->setpoint =
                    joystick_normalize_trigger_position( axis_position,
                                                         setpoint->min_position,
                                                         setpoint->max_position );
            }
        }
    }
    return ( return_code );

}


// *****************************************************
// Function:    is_joystick_safe
//
// Purpose:     Examine the positions of the brake and throttle to determine
//              if they are in a safe position to enable control
//
// Returns:     int - ERROR, NOERR or UNAVAILABLE
//
// Parameters:  void
//
// *****************************************************
static int is_joystick_safe( )
{
    int return_code = ERROR;

    return_code = joystick_update( );

    if ( return_code == NOERR )
    {
        return_code = get_setpoint( &brake_setpoint );

        if ( return_code == NOERR )
        {
            return_code = get_setpoint( &throttle_setpoint );

            if ( return_code == NOERR )
            {
                if ( ( throttle_setpoint.setpoint > 0.0 ) ||
                     ( brake_setpoint.setpoint > 0.0 ) )
                {
                    return_code = UNAVAILABLE;
                }
            }
        }
    }
    return return_code;
}

// *****************************************************
// Function:    commander_set_safe
//
// Purpose:     Put the OSCC module in a safe position
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int commander_set_safe( )
{
    int return_code = ERROR;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = oscc_interface_set_defaults();
    }
    return ( return_code );
}


// *****************************************************
// Function:    calc_exponential_average
//
// Purpose:     Calculate an exponential average based on previous values
//
// Returns:     double - the exponentially averaged result
//
// Parameters:  average - previous average
//              setpoint - new setpoint to incorperate into average
//              factor - factor of exponential average
//
// *****************************************************
static double calc_exponential_average( double average,
                                        double setpoint,
                                        double factor )
{
    double exponential_average =
        ( setpoint * factor ) + ( ( 1 - factor ) * average );

    return ( exponential_average );
}


// *****************************************************
// Function:    commander_disable_controls
//
// Purpose:     Helper function to put the system in a safe state before
//              disabling the OSCC module vehicle controls
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int commander_disable_controls( )
{
    int return_code = ERROR;

    printf( "Disable controls\n" );

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = commander_set_safe( );

        if ( return_code == NOERR )
        {
            return_code = oscc_interface_disable();
        }
    }
    return return_code;
}


// *****************************************************
// Function:    commander_enable_controls
//
// Purpose:     Helper function to put the system in a safe state before
//              enabling the OSCC module vehicle controls
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int commander_enable_controls( )
{
    int return_code = ERROR;

    printf( "Enable controls\n" );

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = commander_set_safe( );

        if ( return_code == NOERR )
        {
            return_code = oscc_interface_enable();
        }
    }
    return ( return_code );
}


// *****************************************************
// Function:    get_button
//
// Purpose:     Wrapper function to get the status of a given button on the
//              joystick
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  button - which button on the joystick to check
//              state - pointer to an unsigned int to store the state of the
//                      button
//
// *****************************************************
static int get_button( unsigned long button, unsigned int* const state )
{
    int return_code = ERROR;

    if ( state != NULL )
    {
        unsigned int button_state;

        return_code = joystick_get_button( button, &button_state );

        if ( ( return_code == NOERR ) &&
             ( button_state == JOYSTICK_BUTTON_STATE_PRESSED ) )
        {
            ( *state ) = 1;
        }
        else
        {
            ( *state ) = 0;
        }
    }
    return ( return_code );
}


// *****************************************************
// Function:    command_brakes
//
// Purpose:     Determine the setpoint being commanded by the joystick and
//              send that value to the OSCC Module
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int command_brakes( )
{
    int return_code = ERROR;
    unsigned int constrained_value = 0;
    static double brake_average = 0.0;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = get_setpoint( &brake_setpoint );

        if ( return_code == NOERR )
        {
            brake_average = calc_exponential_average( brake_average,
                                                      brake_setpoint.setpoint,
                                                      BRAKES_FILTER_FACTOR );

            const float normalized_value = (float) m_constrain(
                (float) brake_average,
                0.0f,
                MAX_BRAKE_PEDAL );

            constrained_value = ( unsigned int ) m_constrain(
                (float) ( normalized_value * (float) UINT16_MAX ),
                (float) 0.0f,
                (float) UINT16_MAX );
        }

        printf( "brake: %d\n", constrained_value );

        return_code = oscc_interface_command_brakes( constrained_value );
    }
    return ( return_code );
}


// *****************************************************
// Function:    command_throttle
//
// Purpose:     Determine the setpoint being commanded by the joystick and
//              send that value to the OSCC Module
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int command_throttle( )
{
    int return_code = ERROR;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = get_setpoint( &throttle_setpoint );

        // don't allow throttle if brakes are applied
        if ( return_code == NOERR )
        {
            return_code = get_setpoint( &brake_setpoint );

            if ( brake_setpoint.setpoint >= BRAKES_ENABLED_MIN )
            {
                throttle_setpoint.setpoint = 0.0;
            }
        }

        // Redundant, but better safe then sorry
        const float normalized_value = (float) m_constrain(
                (float) throttle_setpoint.setpoint,
                0.0f,
                MAX_THROTTLE_PEDAL );

        unsigned int constrained_value = ( unsigned int )m_constrain(
                (float) (normalized_value * (float) UINT16_MAX),
                (float) 0.0f,
                (float) UINT16_MAX );

        printf( "throttle: %d\n", constrained_value );

        return_code = oscc_interface_command_throttle( constrained_value );
    }
    return ( return_code );
}


// *****************************************************
// Function:    command_steering
//
// Purpose:     Determine the setpoint being commanded by the joystick and
//              send that value to the OSCC Module
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int command_steering( )
{
    int return_code = ERROR;
    static double steering_average = 0.0;
    static double last_steering_rate = 0.0;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = get_setpoint( &steering_setpoint );

        steering_average =
            calc_exponential_average( steering_average,
                                      steering_setpoint.setpoint,
                                      STEERING_FILTER_FACTOR );

        const float angle_degrees =
            (float) m_degrees( (float) steering_average );

        const int constrained_angle = ( int ) m_constrain(
                (float) (angle_degrees * STEERING_COMMAND_ANGLE_FACTOR),
                (float) STEERING_COMMAND_ANGLE_MIN,
                (float) STEERING_COMMAND_ANGLE_MAX );

        float rate_degrees =
            (float) fabs( constrained_angle - last_steering_rate );

        last_steering_rate = constrained_angle;

        unsigned int constrained_rate = ( unsigned int ) m_constrain(
                (float) (rate_degrees / (float) STEERING_COMMAND_MAX_VELOCITY_FACTOR),
                (float) STEERING_COMMAND_MAX_VELOCITY_MIN + 1.0f,
                (float) STEERING_COMMAND_MAX_VELOCITY_MAX );

        printf( "steering: %d\t%d\n", constrained_angle, constrained_rate );

        return_code = oscc_interface_command_steering( constrained_angle,
                                                       constrained_rate );
    }
    return ( return_code );
}



// *****************************************************
// public definitions
// *****************************************************

// *****************************************************
// Function:    commander_init
//
// Purpose:     Externally visible function to initialize the commander object
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  channel - for now, the CAN channel to use when interacting
//              with the OSCC modules
//
// *****************************************************
int commander_init( int channel )
{
    int return_code = ERROR;

    if ( commander_enabled == COMMANDER_DISABLED )
    {
        commander_enabled = COMMANDER_ENABLED;

        return_code = oscc_interface_init( channel );

        if ( return_code != ERROR )
        {
            return_code = joystick_init( );

            printf( "waiting for joystick controls to zero\n" );

            while ( return_code != ERROR )
            {
                return_code = is_joystick_safe( );

                if ( return_code == UNAVAILABLE )
                {
                    (void) usleep( JOYSTICK_DELAY_INTERVAL );
                }
                else if ( return_code == ERROR )
                {
                    printf( "Failed to wait for joystick to zero the control values\n" );
                }
                else
                {
                    break;
                }
            }
        }
    }
    return ( return_code );
}

// *****************************************************
// Function:    command_close
//
// Purpose:     Shuts down all of the other modules that the commander uses
//              and closes the commander object
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
void commander_close( )
{
    if ( commander_enabled == COMMANDER_ENABLED )
    {
        commander_disable_controls( );

        oscc_interface_disable( );

        oscc_interface_close( );

        joystick_close( );

        commander_enabled = COMMANDER_DISABLED;
    }
}


// *****************************************************
// Function:    commander_low_frequency_update
//
// Purpose:     Should be run every 50ms
//              The commander low-frequency update function polls the joystick,
//              converts the joystick input into values that reflect what the
//              vehicle should do and sends them to the OSCC interface
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
int commander_low_frequency_update( )
{
    unsigned int button_pressed = 0;

    int return_code = joystick_update( );

    if ( return_code == NOERR )
    {
        return_code = get_button( JOYSTICK_BUTTON_DISABLE_CONTROLS,
                                  &button_pressed );

        if ( return_code == NOERR )
        {
            if ( button_pressed != 0 )
            {
                printf( "Disabling Controls\n" );
                return_code = commander_disable_controls( );
            }
            else
            {
                button_pressed = 0;
                return_code = get_button( JOYSTICK_BUTTON_ENABLE_CONTROLS,
                                          &button_pressed );

                if ( return_code == NOERR )
                {
                    if ( button_pressed != 0 )
                    {
                        return_code = commander_enable_controls( );
                    }
                    else
                    {
                        return_code = command_brakes( );

                        if ( return_code == NOERR )
                        {
                            return_code = command_throttle( );
                        }

                        if ( return_code == NOERR )
                        {
                            return_code = command_steering( );
                        }
                    }
                }
            }
        }
    }
    return return_code;
}


// *****************************************************
// Function:    commander_high_frequency_update
//
// Purpose:     Should be run every 1ms (one millisecond)
//              Run the high-frequency commander tasks
//              Checks the vehicle for override information
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
int commander_high_frequency_update( )
{
    int return_code = ERROR;

    int oscc_override = 0;

    return_code = oscc_interface_update_status( &oscc_override );

    if ( oscc_override == 1 )
    {
        printf( "Driver Override Detected\n" );
        return_code = commander_disable_controls( );
    }
    return return_code;
}
