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
 * @file can_frame.h
 * @brief CAN Frame/Message Definitions.
 *
 */




#ifndef CAN_FRAME_H
#define CAN_FRAME_H


#ifdef __cplusplus
extern "C" {
#endif




#include <stdint.h>




//
#define CAN_FRAME_DLC_MAX (8)




//
typedef struct
{
    //
    //
    uint32_t id;
    //
    //
    uint8_t dlc;
    //
    //
    uint32_t timestamp;
    //
    //
    uint8_t data[CAN_FRAME_DLC_MAX];
} can_frame_s;




#ifdef __cplusplus
}
#endif


#endif	/* CAN_FRAME_H */
