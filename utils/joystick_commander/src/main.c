/**
 * @file main.c
 * @brief Entry point for the joystick commander application
 *
 * Joystick device: Logitech Gamepad F310
 * Mode switch (on back of controller): set to mode X
 * Front mode button: set to off (LED is off)
 * Brake controls: left trigger
 * Throttle controls: right trigger
 * Steering controls: right stick
 * Left turn signal: left trigger button
 * Right turn signal: right trigger button
 * Shift to drive: 'A' button
 * Shift to park: 'Y' button
 * Shift to neutral: 'X' button
 * Shift to reverse: 'B' button
 * Enable controls: 'start' button
 * Disable controls: 'back' button
 *
 */




#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#include "macros.h"
#include "commander.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief update interval. [microseconds]
 *
 * Defines the update rate of the low frequency commander tasks
 *
 * 50,000 us == 50 ms == 20 Hertz
 *
 */
#define COMMANDER_UPDATE_INTERVAL (50000)


/**
 * @brief sleep interval. [microseconds]
 *
 * Specifies the amount of time to sleep for during each wait/sleep cycle
 *
 * Defines the update rate for the high frequency commander tasks
 *
 * Prevents overloading the host CPU
 *
 */
#define SLEEP_TICK_INTERVAL (1000)



// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Error thrown from SIGINT
 *
 */
static int error_thrown = NOERR;


// *****************************************************
// static declarations
// *****************************************************


// *****************************************************
// static definitions
// *****************************************************


// *****************************************************
// Function:    get_timestamp
//
// Purpose:     Get the current timestamp from the system time
//              Value is returned in microseconds
//
// Returns:     unsigned long long - current time in microseconds
//
// Parameters:  void
//
// *****************************************************
static unsigned long long get_timestamp( )
{
    unsigned long long microseconds = 0;
    struct timespec timespec;

    clock_gettime( CLOCK_REALTIME, &timespec );

    // convert to microseconds
    microseconds = (unsigned long long)
        ( 1000000 * ( (unsigned long long) timespec.tv_sec ) );

    // convert nanosecond remainder to micro seconds
    microseconds += (unsigned long long)
        ( ( (unsigned long long) timespec.tv_nsec ) / 1000 );

    return ( microseconds );
}


// *****************************************************
// Function:    get_elapsed_time
//
// Purpose:     Determine the elapsed time since the last check
//
// Returns:     unsigned long long - the elapsed time in microseconds
//
// Parameters:  timestamp - pointer to a timestamp value
//
// *****************************************************
static unsigned long long get_elapsed_time( unsigned long long timestamp )
{
    unsigned long long now = get_timestamp( );
    unsigned long long elapsed_time = now - timestamp;

    return elapsed_time;
}


// *****************************************************
// Function:    signal_handler
//
// Purpose:     Function to catch and handle SIGINT operations
//
// Returns:     void
//
// Parameters:  signal_number - number of the signal sent
//
// *****************************************************
void signal_handler( int signal_number )
{
    if ( signal_number == SIGINT )
    {
        error_thrown = ERROR;
    }
}


// *****************************************************
// public definitions
// *****************************************************


int main( int argc, char **argv )
{
    int return_code = NOERR;
    unsigned long long update_timestamp = get_timestamp();
    unsigned long long elapsed_time = 0;

    int channel;

    errno = 0;

    if ( argc != 2 || ( channel = atoi( argv[1] ), errno ) != 0 )
    {
        printf( "usage %s channel\n", argv[0] );
        exit( 1 );
    }

    signal( SIGINT, signal_handler );

    return_code = commander_init( channel );

    if ( return_code == NOERR )
    {
        while ( return_code == NOERR && error_thrown == NOERR )
        {
            return_code = commander_high_frequency_update( );

            elapsed_time = get_elapsed_time( update_timestamp );

            if ( elapsed_time > COMMANDER_UPDATE_INTERVAL )
            {
                update_timestamp = get_timestamp();
                return_code = commander_low_frequency_update( );
            }

            // Delay 1 ms to avoid loading the CPU and to time calls
            // to commander_high_frequency_update
            (void) usleep( SLEEP_TICK_INTERVAL );
        }
        commander_close( );
    }

    return 0;
}

