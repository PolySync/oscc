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
* @file brake_module_state.h
* @brief Brake module state.
*
**/




#ifndef BRAKE_MODULE_STATE_H
#define BRAKE_MODULE_STATE_H




#include <stdint.h>




/**
 * @brief Brake module state information.
 *
 * Contains state information for the brake module.
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
} brake_module_state_s;




int analyze_brake_state(
        brake_module_state_s * const state,
        const can_frame_s * const brake_command_frame,
        const can_frame_s * const brake_report_frame );




#endif /* BRAKE_MODULE_STATE_H */
