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
* @file gateway_module_state.h
* @brief Gateway module state.
*
**/




#ifndef GATEWAY_MODULE_STATE_H
#define GATEWAY_MODULE_STATE_H




/**
 * @brief Gateway module state information.
 *
 * Contains state information for the gateway module.
 *
 */
typedef struct
{
    //
    //
    uint8_t module_state;
    //
    //
    uint8_t control_state;
    //
    //
    uint8_t override_triggered;
} gateway_module_state_s;




int analyze_gateway_state(
        brake_module_state_s * const state,
        const can_frame_s * heartbeat_msg_frame,
        const can_frame_s * chassis_state1_frame,
        const can_frame_s * chassis_state2_frame );




#endif /* GATEWAY_MODULE_STATE_H */
