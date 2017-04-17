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
* @file system_state.h
* @brief System state.
*
**/




#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H




#include <stdint.h>

#include "control_protocol_can.h"
#include "steering_module_state.h"
#include "throttle_module_state.h"
#include "brake_module_state.h"
#include "gateway_module_state.h"




// Module enabled.
#define CONTROL_ENABLED ( 1 )


// Module disabled.
#define CONTROL_DISABLED ( 0 )


// Module in an OK state
#define STATE_OK ( 2 )


// Module in an WARN state
#define STATE_WARN ( 1 )


// Module in an ERROR state
#define STATE_ERROR ( 0 )


// Module in a non expected state.
#define STATE_FAULT ( -1 )




/**
 * @brief System state information.
 *
 * Contains system state information for whole system.
 *
 */
typedef struct
{
    //
    //
    throttle_module_state_s throttle_module_state;
    //
    //
    steering_module_state_s steering_module_state;
    //
    //
    brake_module_state_s brake_module_state;
    //
    //
    gateway_module_state_s gateway_module_state;
    //
    //
    uint8_t overall_system_state;
    //
    //
    uint8_t overall_system_control_state;
} current_system_state_s;




//
int update_system_state();


//
void print_system_state();




#endif /* SYSTEM_STATE_H */
