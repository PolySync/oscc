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
 * @file kia_obd_can.h
 * @brief Kia Soul OBDII CAN Protocol.
 *
 */




#ifndef KIA_OBD_CAN_H
#define	KIA_OBD_CAN_H


#ifdef __cplusplus
extern "C" {
#endif




#include <stdint.h>




//
#define KIA_CCAN_STATUS1_MESSAGE_ID (0x2B0)


//
#define KIA_CCAN_STATUS2_MESSAGE_ID (0x4B0)


//
#define KIA_CCAN_STATUS3_MESSAGE_ID (0x220)


//
#define KIA_CCAN_STATUS4_MESSAGE_ID (0x18)


//
#define KIA_CCAN_STATUS4_TURN_SIGNAL_LEFT (0x0C)


//
#define KIA_CCAN_STATUS4_TURN_SIGNAL_RIGHT (0x0A)


// ms
#define KIA_CCAN_STATUS1_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS2_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS3_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS4_RX_WARN_TIMEOUT (500)




// 0x2B0
// signals: ...
typedef struct
{
    //
    // 1/10th of a degree per bit
    int16_t steering_angle;
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    //
    uint16_t reserved_2;
} kia_obd_status1_data_s;


// 0x4B0
// signals: ...
typedef struct
{
    //
    // 1/128 mph per bit
    int16_t wheel_speed_lf;
    //
    //
    int16_t wheel_speed_rf;
    //
    //
    int16_t wheel_speed_lr;
    //
    //
    int16_t wheel_speed_rr;
} kia_obd_status2_data_s;


// 0x220
// signals: ...
typedef struct
{
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    // 1/10th of a bar per bit
    int16_t master_cylinder_pressure;
    //
    //
    uint16_t reserved_2;
} kia_obd_status3_data_s;


// 0x18
// signals: ...
typedef struct
{
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    //
    uint8_t reserved_2;
    //
    //
    uint8_t reserved_3 : 4;
    //
    //
    uint8_t turn_signal_flags : 4;
    //
    //
    uint16_t reserved_4;
} kia_obd_status4_data_s;




#ifdef __cplusplus
}
#endif


#endif	/* KIA_OBD_CAN_H */
