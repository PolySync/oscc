/**
 * @file messages.h
 * @brief Message Utilities Interface.
 *
 */




#include "control_protocol_can.h"




#ifndef MESSAGES_H
#define MESSAGES_H




/**
 * @brief Message set.
 *
 * Container for PolySync messages.
 *
 */


typedef struct
{
    //
    //
    ps_ctrl_brake_command_msg brake_cmd; /*!< Platform brake command message. */
    //
    //
    ps_ctrl_throttle_command_msg throttle_cmd; /*!< Platform throttle command message. */
    //
    //
    ps_ctrl_steering_command_msg steering_cmd; /*!< Platform steering wheel command message. */
} messages_s;




//
int messages_is_valid(
        const messages_s * const const messages );


//
int messages_set_default_values(
        messages_s * const messages );




#endif	/* MESSAGES_H */
