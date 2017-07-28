#include <canlib.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <string.h>

#include "macros.h"
#include "can_monitor.h"
#include "terminal_print.h"
#include "system_state.h"




// *****************************************************
// static global types/macros
// *****************************************************


/**
 * @brief Node sleep interval. [microseconds]
 *
 * Specifies the amount of time to sleep for during each wait/sleep cycle.
 *
 * This prevents our node from overloading the host.
 *
 */
#define NODE_SLEEP_TICK_INTERVAL (1000)


/**
 * @brief Interval at which to print information to terminal. [milliseconds]
 *
 * Specifies the amount of time to wait before printing information to terminal.
 *
 * This prevents terminal flickering.
 *
 */
#define PRINT_TERMINAL_TIMEOUT (100)




// *****************************************************
// static global data
// *****************************************************


//
static const char HELP_STRING[] =
"\nOSCC Diagnostics\n\n"
"usage\n"
" $diagnostics-tool [options]\n\n"
"-h\n"
" show this help message [optional]\n\n"
"-c\n"
" set the CAN channel to use [required]\n\n"
"-i\n"
" select CAN ids to do a raw CAN dump [optional]\n"
" if this option is used without arguments the default behaviour is to dump all CAN messages\n"
" surround multiple CAN ids with quotes like so \"135 122 101\"\n\n"
"-d\n"
" run diagnostics and print out system state [optional]\n\n";


//
static int err_thrown = NOERR;


//
static int diagnostics_flag = 0;


//
static int can_channel;


//
static char * can_ids;


//
static int can_id_list[ CAN_MSG_ARRAY_SIZE ];


//
static int num_can_ids = 0;


//
static canHandle can_handle;


//
unsigned long long last_print_timestamp = 0;


//
static int print_can_dump = 0;




// *****************************************************
// static declarations
// *****************************************************


/**
 * @brief Commander update loop.
 *
 * Called by on_warn and on_ok.
 *
 * @param [in] commander A pointer to \ref commander_s which specifies the configuration.
 *
 */
static int update_loop();


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
static int init_can()
{
    int ret = NOERR;

    can_handle = canOpenChannel( can_channel, 0 );//canOPEN_EXCLUSIVE );

    if ( can_handle < 0 )
    {
        ret = ERROR;
        printf( "canOpenChannel %d failed\n", can_channel );
    }

    if( ret == NOERR )
    {
        canBusOff( can_handle );
    }

    if( ret == NOERR )
    {
        ret = check( "canSetBusParams",
                canSetBusParams( can_handle, BAUD_500K, 0, 0, 0, 0, 0 ) );
    }

    if( ret == NOERR )
    {
        ret = check( "canSetBusOutputControl",
            canSetBusOutputControl( can_handle, canDRIVER_NORMAL ) );
    }

    if( ret == NOERR )
    {
        ret = check( "canBusOn", canBusOn( can_handle ) );
    }

    return ret;
}


//
static int check( char* id, canStatus stat )
{
    int ret = NOERR;

    char buf[ 50 ];

    buf[ 0 ] = '\0';

    canGetErrorText( stat, buf, sizeof( buf ) );

    if ( stat != canOK )
    {
        ret = ERROR;
        printf( "%s: failed, stat=%d (%s)\n", id, (int) stat, buf );
    }
    else
    {
        printf( "%s: OK\n", id );
    }

    return ret;
}


//
static int read_from_buffer( canHandle handle )
{
    // local vars
    int ret = NOERR;
    canStatus retval = canOK;

    long can_id;
    unsigned int msg_dlc;
    unsigned int msg_flag;
    unsigned long tstamp;

    unsigned char buffer[ 8 ];


    retval = canRead( handle,
            &can_id,
            buffer,
            &msg_dlc,
            &msg_flag,
            &tstamp );


    if( retval == canOK )
    {
        ret = handle_can_rx( can_id, msg_dlc, msg_flag, tstamp, buffer );
    }
    else if( (retval == canERR_NOMSG) || (retval == canERR_TIMEOUT) )
    {
        // Do nothing
    }
    else
    {
        ret = ERROR;
    }


    return ret;
}


//
static void sig_handler( int signo )
{
    if ( signo == SIGINT )
    {
        err_thrown = ERROR;
    }
}


//
static int update_loop()
{
    int ret = NOERR;

    ret = read_from_buffer( can_handle );


    if( get_timestamp() - last_print_timestamp > PRINT_TERMINAL_TIMEOUT )
    {
        if( print_can_dump )
        {
            print_can_array( can_id_list, num_can_ids );
        }

        if( diagnostics_flag )
        {
            update_system_state();

            print_system_state();
        }

        print_lines();

        last_print_timestamp = get_timestamp();
    }

    // sleep for 1 ms to avoid loading the CPU
    (void) usleep( NODE_SLEEP_TICK_INTERVAL );

    return ret;
}


//
static int handle_get_opt( int argc, char **argv )
{
    int ret = NOERR;

    int c;

    int c_option = 0;

    while( ( c = getopt ( argc, argv, "idhc:" ) ) != -1 )
    {
        switch ( c )
        {
            case 'i':
                can_ids = optarg;

                print_can_dump = 1;

                break;

            case 'd':
                diagnostics_flag = 1;

                break;

            case 'c':
                can_channel = atoi( optarg );

                c_option = 1;

                break;

            case 'h':
                printf( HELP_STRING );

                // c option error message is not necessary
                c_option = 1;

                ret = UNAVAILABLE;

                break;

            case '?':
                ret = ERROR;

                break;

            default:
                continue;
        }
    }

    if( c_option == 0 )
    {
        printf( "Option -c is required, please see help message (-h)\n" );

        ret = ERROR;
    }

    return ret;
}


//
static void process_can_ids()
{
    char * split_ids = strtok( can_ids, " " );

    while( split_ids != NULL && num_can_ids < CAN_MSG_ARRAY_SIZE )
    {
        can_id_list[ num_can_ids ] = atoi( split_ids );

        num_can_ids++;

        split_ids = strtok( NULL, " " );
    }
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
    if( handle_get_opt( argc, argv ) != NOERR )
    {
        exit( 1 );
    }

    signal( SIGINT, sig_handler );

    if( ret == NOERR )
    {
        ret = init_can();
    }

    process_can_ids();

    init_can_msg_array();

    while( ret == NOERR && err_thrown == NOERR )
    {
        // do update loop
        if( ret == NOERR && err_thrown == NOERR )
        {
            ret = update_loop();
        }
    }

    return 0;
}
