/************************************************************************/
/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */
/*                                                                      */
/* This file is part of Open Source Car Control (OSCC).                 */
/*                                                                      */
/* OSCC is free software: you can redistribute it and/or modify         */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation, either version 3 of the License, or    */
/* (at your option) any later version.                                  */
/*                                                                      */
/* OSCC is distributed in the hope that it will be useful,              */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* You should have received a copy of the GNU General Public License    */
/* along with OSCC.  If not, see <http://www.gnu.org/licenses/>.        */
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
