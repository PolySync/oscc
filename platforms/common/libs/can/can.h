#ifndef _OSCC_CAN_H_
#define _OSCC_CAN_H_


#include <stdint.h>
#include "mcp_can.h"


#define CAN_BAUD (CAN_500KBPS)
#define CAN_INIT_RETRY_DELAY (50)
#define CAN_FRAME_DLC_MAX (8)


typedef enum
{
    CAN_RX_FRAME_AVAILABLE = 0,
    CAN_RX_FRAME_UNAVAILABLE,
    CAN_RX_FRAME_UNKNOWN
} can_status_t;


typedef struct
{
    uint32_t id;
    uint8_t dlc;
    uint32_t timestamp;
    uint8_t data[CAN_FRAME_DLC_MAX];
} can_frame_s;


// *****************************************************
// Function:    init_can
//
// Purpose:     Initializes the CAN interface.
//              Function must iterate while the CAN module initializes.
//
// Returns:     void
//
// Parameters:  [in] can - An MCP_CAN object.
//
// *****************************************************
void init_can(
    MCP_CAN &can );


// *****************************************************
// Function:    check_for_rx_frame
//
// Purpose:     Checks for CAN frame and stores it if there is one.
//
// Returns:     can_status_t - Status code indicating whether a frame was received.
//
// Parameters:  [in] can - An MCP_CAN object.
//              [out] frame - A can_frame_s struct in which to store the frame.
//
// *****************************************************
can_status_t check_for_rx_frame(
    MCP_CAN &can,
    can_frame_s * const frame );


#endif
