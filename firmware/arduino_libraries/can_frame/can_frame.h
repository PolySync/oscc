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
