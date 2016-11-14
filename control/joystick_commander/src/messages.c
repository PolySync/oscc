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
 * @file messages.c
 * @brief Message Utilities Interface Source.
 *
 */




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"
#include "messages.h"




// *****************************************************
// static global types/macros
// *****************************************************




// *****************************************************
// static global data
// *****************************************************




// *****************************************************
// static definitions
// *****************************************************


//
static int set_default_brake_command(
        ps_ctrl_brake_command_msg * const msg )
{
    int ret = NOERR;


    if( msg == NULL )
    {
        ret = ERROR;
    }
    else
    {
        msg->brake_on = 0;

        msg->clear = 0;

        msg->count = 0;

        msg->enabled = 0;

        msg->ignore = 0;

        msg->pedal_command = 0;
    }
    

    return ret;
}


//
static int set_default_throttle_command(
        ps_ctrl_throttle_command_msg * const msg )
{
    int ret = NOERR;


    if( msg == NULL )
    {
        ret = ERROR;
    }
    else
    {
        msg->clear = 0;
        
        msg->count = 0;
        
        msg->enabled = 0;
        
        msg->ignore = 0;
        
        msg->pedal_command = 0;
    }


    return ret;
}


//
static int set_default_steering_command(
        ps_ctrl_steering_command_msg * const msg )
{
    int ret = NOERR;


    if( msg == NULL )
    {
        ret = ERROR;
    }
    else
    {
        msg->clear = 0;
        
        msg->count = 0;
        
        msg->enabled = 0;
        
        msg->ignore = 0;
        
        msg->steering_wheel_angle_command = 0;
        
        msg->steering_wheel_max_velocity = 0;
    }


    return ret;
}




// *****************************************************
// public definitions
// *****************************************************


//
int messages_is_valid(
        const messages_s * const const messages )
{
    int ret = NOERR;


    if( messages == NULL )
    {
        ret = ERROR;
    }


    return ret;
}


//
int messages_set_default_values(
        messages_s * const messages )
{
    int ret = NOERR;


    if( messages == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // default brake command values
        ret |= set_default_brake_command(
                &messages->brake_cmd );

        // default throttle command values
        ret |= set_default_throttle_command(
                &messages->throttle_cmd );

        // default steering command values
        ret |= set_default_steering_command(
                &messages->steering_cmd );
    }


    return ret;
}
