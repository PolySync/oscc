/**
 * @file node.c
 * @brief Node Layer Source.
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




#include <canlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <time.h>

#include "macros.h"
#include "joystick.h"
#include "messages.h"
#include "commander.h"
#include "control_protocol_can.h"




// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief node update and publish interval. [microseconds]
 *
 * Defines the update and publish rate of the node.
 *
 * 50,000 us == 50 ms == 20 Hertz
 *
 */
#define NODE_UPDATE_INTERVAL (50000)


/**
 * @brief Node sleep interval. [microseconds]
 *
 * Specifies the amount of time to sleep for during each wait/sleep cycle.
 *
 * This prevents our node from overloading the host.
 *
 */
#define NODE_SLEEP_TICK_INTERVAL (1000)



// *****************************************************
// static global data
// *****************************************************

/**
 * @brief Warning string.
 *
 */
static const char WARNING_STRING[] =
"\nWARNING: example is built for "
"the Joystick device: Logitech Gamepad F310\n"
"Back mode switch: 'X' setting\n"
"Front mode button: off (LED is off)"
"Brake controls: left trigger\n"
"Throttle controls: right trigger\n"
"Steering controls: right stick\n"
"Left turn signal: left trigger button\n"
"Right turn signal: right trigger button\n"
"Shift to drive: 'A' button\n"
"Shift to park: 'Y' button\n"
"Shift to neutral: 'X' button\n"
"Shift to reverse: 'B' button\n"
"Enable controls: 'start' button\n"
"Disable controls: 'back' button\n\n";


/**
 * @brief Error thrown from SIGINT.
 *
 */
static int err_thrown = NOERR;




// *****************************************************
// static declarations
// *****************************************************


//
static unsigned long long get_time_since(
        const unsigned long long past,
        unsigned long long * const now );


/**
 * @brief Commander update loop.
 *
 * Called by on_warn and on_ok.
 *
 * @param [in] commander A pointer to \ref commander_s which specifies the configuration.
 *
 */
static int update_loop(
        commander_s * const commander );


/**
 * @brief CAN check.
 *
 * Check the error code returned by CAN functions.
 *
 * @param [in] id Error ID.
 * @param [in] stat Current CAN status.
 *
 */
static int check ( char* id, canStatus stat );




// *****************************************************
// static definitions
// *****************************************************


//
static int init_can( int can_channel_index, canHandle* handle )
{
    int ret = NOERR;
    
    if( handle == NULL )
    {
        ret = ERROR;
    }
    
    if( ret == NOERR )
    {
        (*handle) = canOpenChannel( can_channel_index, 0x40 );

        if ( (*handle) < 0 ) 
        {
            ret = ERROR;
            printf( "canOpenChannel %d failed\n", can_channel_index );
        }
    }

    if( ret == NOERR )
    {
        canBusOff( (*handle) );
    }
    
    if( ret == NOERR )
    {
        ret = check( "canSetBusParams", canSetBusParams( (*handle), BAUD_500K, 0, 0, 0, 0, 0 ) );
    }
    
    if( ret == NOERR )
    {
        ret = check( "canSetBusOutputControl", canSetBusOutputControl( (*handle), canDRIVER_NORMAL ) );
    }
    
    if( ret == NOERR )
    {    
        ret = check( "canBusOn", canBusOn( (*handle) ) );
    }
    
    return ret;
}


//
static int init_messages(
        commander_s * commander )
{
    int ret = NOERR;

    // check reference since other routines don't
    if( commander == NULL )
    {
        ret = ERROR;
    }

    if( ret == NOERR )
    {
        // allocate messages
        ret = messages_alloc( &commander->messages );
    }
    
    if( ret == NOERR )
    {
        // set safe state
        ret = commander_set_safe( commander );
    }
    
    return ret;
}


//
static int init_joystick( commander_s * commander )
{
    int ret = NOERR;
    
    ret = jstick_init_subsystem();
    
    if( ret == ERROR )
        printf("init subsystem error\n");    

    // get number of joysticks visible
    const int num_joysticks = jstick_get_num_devices();

    // if any are available
    if( num_joysticks > 0 && ret == NOERR )
    {
        // device GUID
        joystick_guid_s js_guid;

        // default device
        const unsigned long default_device_index = 0;

        // get GUID of device at index
        ret = jstick_get_guid_at_index( 0, &js_guid );

        if( ret == NOERR )
        {
            printf(
                    "Found %d devices -- connecting to device at system index %lu - GUID: %s\n",
                    num_joysticks,
                    default_device_index,
                    js_guid.ascii_string );

            // connect to first device
            ret = jstick_open(
                    default_device_index,
                    &commander->joystick );
        }

        // wait for safe state
        printf( "waiting for joystick controls to zero\n" );
        while( ret != ERROR )
        {
            ret = commander_check_for_safe_joystick( commander );

            if( ret == UNAVAILABLE )
            {
                // wait a little for the next try
                (void) usleep( NODE_UPDATE_INTERVAL );
            }
            else if( ret == ERROR )
            {
                printf( "Failed to wait for joystick to zero the control values\n" );
            }
            else
            {
                break;
            }
        }
    }
    else
    {
        printf( "No joystick/devices available on the host\n" );
    }
}


//
//
static int get_timestamp( unsigned long long * const timestamp )
{
    if( timestamp == NULL )
    {
        return ERROR;
    }

    // local vars
    struct timespec timespec;

    // get time
    clock_gettime( CLOCK_REALTIME, &timespec );

    // convert to micro seconds
    (*timestamp) = (unsigned long long) 1000000*( (unsigned long long) timespec.tv_sec );

    // convert nanosecond remainder to micro seconds
    (*timestamp) += (unsigned long long) ( (unsigned long long) timespec.tv_nsec )/1000;


    return NOERR;
}


//
static unsigned long long get_time_since(
        const unsigned long long past,
        unsigned long long * const now )
{
    unsigned long long delta = 0;
    unsigned long long m_now = 0;


    const int ret = get_timestamp( &m_now );

    if( ret == NOERR )
    {
        if( m_now >= past )
        {
            delta = (m_now - past);
        }
    }

    // update provided argument if valid
    if( now != NULL )
    {
        (*now) = m_now;
    }


    return delta;
}


//
static int check (char* id, canStatus stat)
{
    int ret = NOERR;
    
    char buf[50];

    buf[0] = '\0';

    canGetErrorText(stat, buf, sizeof(buf));

    if (stat != canOK) 
    {
        ret = ERROR;
        printf("%s: failed, stat=%d (%s)\n", id, (int)stat, buf);
    } 
    else 
    {
        printf("%s: OK\n", id);
    }
    
    return ret;
}


//
void sig_handler( int signo )
{
    if ( signo == SIGINT )
    {
        err_thrown = ERROR;
    }
}


//
static int update_loop(
        commander_s * const commander )
{
    int ret = NOERR;
    
    unsigned long long now = 0;


    // get the amount of time since our last update/publish
    const unsigned long long time_since_last_publish =
            get_time_since( commander->last_commander_update, &now );

    // if we have a valid destination node GUID
    
    // only update/publish at our defined interval
    if( time_since_last_publish >= NODE_UPDATE_INTERVAL )
    {
        // update commander, send command messages
        ret = commander_update( commander );

        commander->last_commander_update = now;
    }

    // sleep for 1 ms to avoid loading the CPU
    (void) usleep( NODE_SLEEP_TICK_INTERVAL );
    
    return ret;
}




// *****************************************************
// public definitions
// *****************************************************

//
int main( int argc, char **argv )
{
    int ret = NOERR;
    
    int channel;
    
    errno = 0;
    if ( argc != 2 || ( channel = atoi( argv[1] ), errno ) != 0 ) 
    {
        printf( "usage %s channel\n", argv[0] );
        exit( 1 );
    }
    
    signal( SIGINT, sig_handler );
    
    commander_s * const commander = ( commander_s* ) malloc( sizeof( commander_s ) );

    ret = init_messages( commander );
    
    if( ret == NOERR )
    {
        ret = init_can( channel, &commander->canhandle );
    }
    
    if( ret == NOERR )
    {
        ret = init_joystick( commander );
    }
    
    while( ret == NOERR && err_thrown == NOERR )
    {
        // check if command data is valid
        ret = commander_is_valid( commander );

        // do update loop
        if( ret == NOERR && err_thrown == NOERR )
        {
            ret = update_loop( commander );
        }
    }

    if( commander_is_valid( commander ) == NOERR )
    {
        // send command to disable controls
        (void) commander_disable_controls( commander );

        // make sure disable messages have been sent
        check( "canWriteSync", canWriteSync( commander->canhandle, 1000 ) );

        // close device if needed
        jstick_close( &commander->joystick );

        // free messages
        (void) messages_free( &commander->messages );

        free( commander );
    }

    // release joystick subsystem
    jstick_release_subsystem();
    
    return 0;
}