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
 * @file can_monitor.c
 * @brief Can monitor interface source.
 *
 */




#include <sys/time.h>
#include <stdlib.h>
#include <canlib.h>

#include "messages.h"
#include "macros.h"
#include "can_monitor.h"
//#include "commander.h"




// *****************************************************
// static global data
// *****************************************************


static can_frame_s can_msg_array[ CAN_MSG_ARRAY_SIZE ];




// *****************************************************
// static declarations
// *****************************************************


//
static bool update_can_array_contents(
        const unsigned int msg_dlc,
        const unsigned int msg_flag,
        const unsigned char * const buffer,
        can_frame_contents_s * const can_frame_contents )
{
    bool msg_contents_changed = false;

    can_frame_contents->msg_dlc = msg_dlc;

    can_frame_contents->msg_flag = msg_flag;

    for( int i = 0; i < 8; i ++ )
    {
        if( can_frame_contents->buffer[ i ] != buffer[ i ] )
        {
            msg_contents_changed = true;

            can_frame_contents->buffer[ i ] = buffer[ i ];
        }
    }

    return msg_contents_changed;
}


//
static bool update_can_array_msg(
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
            &can_msg_array[ msg_array_index ].can_frame_contents );
}


//
static int get_can_msg_array_index( const long can_id )
{
    int msg_array_index = -1;

    for( int i = 0; i < CAN_MSG_ARRAY_SIZE; i ++ )
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

    int new_freq = 1000 / deltaT_message_arrival;

    return new_freq * factor + ( 1 - factor ) * msg_freq;
}


//
static int calc_msg_timestamp_freq(
        const int deltaT_message_arrival,
        const int array_index )
{
    const float factor = 0.4;

    int msg_freq = can_msg_array[ array_index ].msg_timestamp_frequency;

    int new_freq = 1000 / deltaT_message_arrival;

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

    int msg_timestamp_deltaT_arrival =
            tstamp -
            can_msg_array[ array_index ].frame_contents.tstamp;

    can_msg_array[ array_index ].last_msg_deltaT = deltaT_arrival;

    can_msg_array[ array_index ].last_msg_timestamp_deltaT =
            msg_timestamp_deltaT_arrival;

    can_msg_array[ array_index ].last_arrival_timestamp = deltaT_arrival;

    can_msg_array[ array_index ].msg_frequency =
            calc_msg_freq( deltaT_message_arrival );

    can_msg_array[ array_index ].msg_timestamp_frequency =
            calc_msg_timestamp_freq( msg_timestamp_deltaT_arrival );
}


//
static int handle_can_rx(
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
        bool new_msg_contents =
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




// *****************************************************
// declarations
// *****************************************************


//
unsigned long long get_timestamp()
{
    gettimeofday( &tv, NULL );

    return ( unsigned long long )( tv.tv_sec ) * 1000 +
            ( unsigned long long )( tv.tv_usec ) / 1000;
}


//
void init_can_msg_array()
{
    for( int i = 0; i < CAN_MSG_ARRAY_SIZE; i ++ )
    {
        can_msg_array[ i ].can_id = 0;
    }
}


//
void print_can_array( int * can_id_print_list, int num_can_ids )
{
    for( int i = 0; i < CAN_MSG_ARRAY_SIZE; i++ )
    {
        for( int j = 0; j < num_can_ids; j++ )
        {
            if( can_id_print_list[ j ] == can_msg_array[ i ].can_id )
            {
                printf( "can id: %d, msg freq: %d, timestamp freq: %d, last arrival deltaT: %d, last timestamp deltaT: %d\n",
                        can_msg_array[ i ].can_id,
                        can_msg_array[ i ].msg_frequency,
                        can_msg_array[ i ].msg_timestamp_frequency,
                        can_msg_array[ i ].last_msg_deltaT,
                        can_msg_array[ i ].last_msg_timestamp_deltaT );
            }
        }
    }
}


//
int can_read_buffer( canHandle handle )
{
    // local vars
    int ret = NOERR;
    canStatus retval = canOK;

    long can_id;
    unsigned int msg_dlc;
    unsigned int msg_flag;
    unsigned long tstamp;

    unsigned char buffer[ 8 ];


    retval = canRead( commander->canhandle,
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
