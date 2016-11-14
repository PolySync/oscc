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
