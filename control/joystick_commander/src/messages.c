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