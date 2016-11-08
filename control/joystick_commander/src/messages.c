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
int messages_alloc(
        messages_s * const messages )
{
    int ret = NOERR;


    if( messages == NULL )
    {
        ret = ERROR;
    }
    else
    {
        // get brake command message
        if( ret )
        {
            messages->brake_cmd = ( ps_ctrl_brake_command_msg* ) malloc( sizeof( ps_ctrl_brake_command_msg ) );
        }

        // get throttle command message
        if( ret )
        {
            messages->throttle_cmd = ( ps_ctrl_throttle_command_msg* ) malloc( sizeof( ps_ctrl_throttle_command_msg ) );
        }

        // get steering command message
        if( ret )
        {
            messages->steering_cmd = ( ps_ctrl_steering_command_msg* ) malloc( sizeof( ps_ctrl_steering_command_msg ) );
        }

        // default values
        if( ret )
        {
            ret = messages_set_default_values(
                    messages );
        }
    }


    return ret;
}


//
int messages_free(
        messages_s * const messages )
{
    int ret = NOERR;


    if( messages == NULL )
    {
        ret = ERROR;
    }
    else
    {
        free( messages->brake_cmd );

        free( messages->throttle_cmd );

        free( messages->steering_cmd );
        
    }


    return ret;
}


//
int messages_is_valid(
        const messages_s * const const messages )
{
    int ret = NOERR;


    if( messages == NULL )
    {
        ret = ERROR;
    }
    else
    {
        if( messages->brake_cmd == NULL )
        {
            ret = ERROR;
        }

        if( messages->throttle_cmd == NULL )
        {
            ret = ERROR;
        }

        if( messages->steering_cmd == NULL )
        {
            ret = ERROR;
        }
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
                messages->brake_cmd );

        // default throttle command values
        ret |= set_default_throttle_command(
                messages->throttle_cmd );

        // default steering command values
        ret |= set_default_steering_command(
                messages->steering_cmd );
    }


    return ret;
}