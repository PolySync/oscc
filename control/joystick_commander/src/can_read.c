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
 * @file can_read.c
 * @brief Can read interface source.
 *
 */




#include <stdlib.h>
#include <canlib.h>

#include "messages.h"
#include "macros.h"
#include "can_read.h"
#include "commander.h"




//
int handle_can_rx_brake_report( 
        ps_ctrl_brake_report_msg * brake_report,
        int * driver_override_flag )
{
    int ret = NOERR;
    
    if( (brake_report == NULL) || (driver_override_flag == NULL) )
    {
        ret = ERROR;
    }
    
    if( ret == NOERR )
    {
        if( (*driver_override_flag) == 0 )
        {
            (*driver_override_flag) = brake_report->override;
        }
    }
    
    return ret;
}


//
int handle_can_rx_throttle_report( 
        ps_ctrl_throttle_report_msg * throttle_report,
        int * driver_override_flag )
{
    int ret = NOERR;
    
    if( (throttle_report == NULL) || (driver_override_flag == NULL) )
    {
        ret = ERROR;
    }
    
    if( ret == NOERR )
    {
        if( (*driver_override_flag) == 0 )
        {
            (*driver_override_flag) = throttle_report->override; 
        }
    }
    
    return ret;
}


//
int handle_can_rx_steering_report( 
        ps_ctrl_steering_report_msg * steering_report,
        int * driver_override_flag )
{
    int ret = NOERR;
    
    if( (steering_report == NULL) || (driver_override_flag == NULL) )
    {
        ret = ERROR;
    }
    
    if( ret == NOERR )
    {
        if( (*driver_override_flag) == 0 )
        {
            (*driver_override_flag) = steering_report->override;
        }
    }
    
    return ret;
}


//
int can_read_buffer( 
        commander_s * const commander )
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
        // handle supported frame IDs
        if( can_id == PS_CTRL_MSG_ID_BRAKE_REPORT )
        {
            // handle brake report frame
            ret = handle_can_rx_brake_report( 
                    (ps_ctrl_brake_report_msg*) buffer,
                    &commander->driver_override );
        }
        else if( can_id == PS_CTRL_MSG_ID_THROTTLE_REPORT )
        {
            // handle throttle report frame
            ret = handle_can_rx_throttle_report(
                    (ps_ctrl_throttle_report_msg*) buffer,
                    &commander->driver_override );
        }
        else if( can_id == PS_CTRL_MSG_ID_STEERING_REPORT )
        {
            // handle steering report frame
            ret = handle_can_rx_steering_report(
                    (ps_ctrl_steering_report_msg*) buffer,
                    &commander->driver_override );
        }
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