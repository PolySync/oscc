/************************************************************************/
/* The MIT License (MIT)                                                */
/* =====================                                                */
/*                                                                      */
/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* Permission is hereby granted, free of charge, to any person          */
/* obtaining a copy of this software and associated documentation       */
/* files (the “Software”), to deal in the Software without              */
/* restriction, including without limitation the rights to use,         */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell    */
/* copies of the Software, and to permit persons to whom the            */
/* Software is furnished to do so, subject to the following             */
/* conditions:                                                          */
/*                                                                      */
/* The above copyright notice and this permission notice shall be       */
/* included in all copies or substantial portions of the Software.      */
/*                                                                      */
/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND,      */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES      */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND             */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT          */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,         */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING         */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR        */
/* OTHER DEALINGS IN THE SOFTWARE.                                      */
/************************************************************************/

/**
 * @file can_read.h
 * @brief Can read interface.
 *
 */




#ifndef CAN_READ_H
#define CAN_READ_H




#include <canlib.h>




#define CAN_MSG_ARRAY_SIZE ( 100 )




/**
 * @brief CAN message data.
 *
 * Serves as a container for incoming CAN message body.
 *
 */
typedef struct
{
    //
    //
    unsigned int msg_dlc;
    //
    //
    unsigned int msg_flag;
    //
    //
    unsigned long tstamp;
    //
    //
    unsigned char buffer[ 8 ];
} can_frame_contents_s;


/**
 * @brief CAN message data.
 *
 * Serves as a container for incoming CAN messages.
 *
 */
typedef struct
{
    //
    //
    long can_id;
    //
    //
    unsigned long long last_arrival_timestamp;
    //
    //
    unsigned int msg_frequency;
    //
    //
    unsigned int last_msg_deltaT;
    //
    //
    unsigned int msg_timestamp_frequency;
    //
    //
    unsigned int last_msg_timestamp_deltaT;
    //
    //
    can_frame_contents_s frame_contents;
} can_frame_s;




//
unsigned long long get_timestamp( );


//
void init_can_msg_array( );


//
const can_frame_s * const get_can_msg_array_index_reference(
        const long can_id );


//
void print_can_array( int * can_id_print_list, int num_can_ids );


//
int handle_can_rx(
        const long can_id,
        const unsigned int msg_dlc,
        const unsigned int msg_flag,
        const unsigned long tstamp,
        const unsigned char * const buffer );




#endif /* CAN_READ_H */
