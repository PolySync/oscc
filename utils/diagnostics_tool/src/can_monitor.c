/**
 * @file can_monitor.c
 * @brief Can monitor interface source.
 *
 */




#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "terminal_print.h"
#include "can_monitor.h"




// *****************************************************
// static global data
// *****************************************************


//
static can_frame_s can_msg_array[ CAN_MSG_ARRAY_SIZE ];


//
static struct timeval tv;




// *****************************************************
// static definitions
// *****************************************************


//
static int update_can_array_contents(
        const unsigned int msg_dlc,
        const unsigned int msg_flag,
        const unsigned char * const buffer,
        can_frame_contents_s * const can_frame_contents )
{
    int i;

    int msg_contents_changed = 0;

    can_frame_contents->msg_dlc = msg_dlc;

    can_frame_contents->msg_flag = msg_flag;

    if( memcmp( can_frame_contents->buffer, buffer, 8 ) != 0 )
    {
        msg_contents_changed = 1;

        memcpy( can_frame_contents->buffer, buffer, 8 );
    }

    return msg_contents_changed;
}


//
static int update_can_array_msg(
        const int msg_array_index,
        const long can_id,
        const unsigned int msg_dlc,
        const unsigned int msg_flag,
        const unsigned char * const buffer )
{
    can_msg_array[ msg_array_index ].can_id = can_id;

    return update_can_array_contents(
            msg_dlc,
            msg_flag,
            buffer,
            &can_msg_array[ msg_array_index ].frame_contents );
}


//
static int get_can_msg_array_index( const long can_id )
{
    int i;

    int msg_array_index = -1;

    for( i = 0; i < CAN_MSG_ARRAY_SIZE; i ++ )
    {
        // empty array index found, or CAN message index found
        if( can_msg_array[ i ].can_id == 0 ||
                can_msg_array[ i ].can_id == can_id )
        {
            msg_array_index = i;

            break;
        }
    }

    return msg_array_index;
}


//
static int calc_msg_freq(
        const int deltaT_message_arrival,
        const int array_index )
{
    const float factor = 0.4;

    int msg_freq = can_msg_array[ array_index ].msg_frequency;

    int new_freq = deltaT_message_arrival;//1000 /deltaT_message_arrival;

    return new_freq * factor + ( 1 - factor ) * msg_freq;
}


//
static int calc_msg_timestamp_freq(
        const int deltaT_message_arrival,
        const int array_index )
{
    const float factor = 0.4;

    int msg_freq = can_msg_array[ array_index ].msg_timestamp_frequency;

    int new_freq = deltaT_message_arrival; //1000 / deltaT_message_arrival;

    return new_freq * factor + ( 1 - factor ) * msg_freq;
}


//
static void update_can_array_timestamp(
        const int array_index,
        const unsigned long tstamp )
{
    int deltaT_arrival =
            get_timestamp() -
            can_msg_array[ array_index ].last_arrival_timestamp;

    can_msg_array[ array_index ].last_arrival_timestamp = get_timestamp();

    int msg_timestamp_deltaT_arrival =
            tstamp -
            can_msg_array[ array_index ].frame_contents.tstamp;

    can_msg_array[ array_index ].frame_contents.tstamp = tstamp;

    can_msg_array[ array_index ].last_msg_deltaT = deltaT_arrival;

    can_msg_array[ array_index ].last_msg_timestamp_deltaT =
            msg_timestamp_deltaT_arrival;

    can_msg_array[ array_index ].msg_frequency =
            calc_msg_freq( deltaT_arrival, array_index );

    can_msg_array[ array_index ].msg_timestamp_frequency =
            calc_msg_timestamp_freq( msg_timestamp_deltaT_arrival, array_index );
}


//
static void print_info_can_array_index( int index )
{
    char print_array[ 500 ];

    sprintf( print_array,
        "can id: %ld, msg freq: %u, timestamp freq: %u, last arrival deltaT: %u, last timestamp deltaT: %u",
        can_msg_array[ index ].can_id,
        can_msg_array[ index ].msg_frequency,
        can_msg_array[ index ].msg_timestamp_frequency,
        can_msg_array[ index ].last_msg_deltaT,
        can_msg_array[ index ].last_msg_timestamp_deltaT );

    add_line( print_array );
}




// *****************************************************
// public definitions
// *****************************************************


//
unsigned long long get_timestamp()  // milliseconds
{
    gettimeofday( &tv, NULL );

    return ( unsigned long long )( tv.tv_sec ) * 1000 +
            ( unsigned long long )( tv.tv_usec ) / 1000;
}


//
void init_can_msg_array()
{
    int i;

    for( i = 0; i < CAN_MSG_ARRAY_SIZE; i ++ )
    {
        can_msg_array[ i ].can_id = 0;
    }
}


//
const can_frame_s * const get_can_msg_array_index_reference(
        const long can_id )
{
    const can_frame_s * frame_pointer;

    int array_index = get_can_msg_array_index( can_id );

    if( array_index == -1 )
    {
        frame_pointer = NULL;
    }
    else
    {
        frame_pointer = &can_msg_array[ array_index ];
    }

    return frame_pointer;
}


//
void print_can_array( int * can_id_print_list, int num_can_ids )
{
    int i, j;

    int print_all_ids = 0;

    if( num_can_ids == 0 )
    {
        print_all_ids = 1;
    }

    add_line( "" );
    add_line( "Raw CAN dump:" );

    for( i = 0; i < CAN_MSG_ARRAY_SIZE; i++ )
    {
        if( print_all_ids && can_msg_array[ i ].can_id > 0 )
        {
            print_info_can_array_index( i );
        }
        else
        {
            for( j = 0; j < num_can_ids; j++ )
            {
                if( can_id_print_list[ j ] == can_msg_array[ i ].can_id )
                {
                    print_info_can_array_index( i );
                }
            }
        }
    }
}


//
int handle_can_rx(
        const long can_id,
        const unsigned int msg_dlc,
        const unsigned int msg_flag,
        const unsigned long tstamp,
        const unsigned char * const buffer )
{
    int ret = NOERR;

    int can_msg_array_index = get_can_msg_array_index( can_id );

    if( can_msg_array_index == -1 )
    {
        ret = ERROR;
    }

    if( ret == NOERR )
    {
        int new_msg_contents =
                update_can_array_msg(
                        can_msg_array_index,
                        can_id,
                        msg_dlc,
                        msg_flag,
                        buffer );

        update_can_array_timestamp(
                can_msg_array_index,
                tstamp );
    }

    return ret;
}