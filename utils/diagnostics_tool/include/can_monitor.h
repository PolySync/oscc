/**
 * @file can_read.h
 * @brief Can read interface.
 *
 */




#ifndef CAN_READ_H
#define CAN_READ_H




#include <canlib.h>




#define CAN_MSG_ARRAY_SIZE 100




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
unsigned long long get_timestamp();


//
void init_can_msg_array();


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
