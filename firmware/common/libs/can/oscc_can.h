/**
 * @file can.h
 * @brief CAN interface.
 *
 */


#ifndef _OSCC_CAN_H_
#define _OSCC_CAN_H_


#include <stdint.h>

#include "mcp_can.h"


/*
 * @brief CAN baudrate.
 *
 */
#define CAN_BAUD (CAN_500KBPS)

/*
 * @brief Delay between CAN initialization attempts.
 *
 */
#define CAN_INIT_RETRY_DELAY (50)

/*
 * @brief Maximum size of CAN frame data.
 *
 */
#define CAN_FRAME_DLC_MAX (8)

/*
 * @brief Standard CAN frame length (vs extended).
 *
 */
#define CAN_STANDARD (0)


/*
 * @brief Return values when checking for a received can frame.
 *
 */
typedef enum
{
    CAN_RX_FRAME_AVAILABLE = 0, /* CAN frame available on bus. */

    CAN_RX_FRAME_UNAVAILABLE, /* No CAN frame on bus. */

    CAN_RX_FRAME_UNKNOWN /* CAN frame status unknown. */
} can_status_t;


/*
 * @brief Standard CAN frame.
 *
 */
typedef struct
{
    uint32_t id; /* CAN frame ID. */

    uint8_t dlc; /* CAN frame data length. */

    uint32_t timestamp; /* Timestamp when CAN frame was received. */

    uint8_t data[CAN_FRAME_DLC_MAX]; /* CAN frame data. */
} can_frame_s;


// ****************************************************************************
// Function:    init_can
//
// Purpose:     Initializes the CAN interface.
//              Function must iterate while the CAN module initializes.
//
// Returns:     void
//
// Parameters:  [in] can - An MCP_CAN object.
//
// ****************************************************************************
void init_can(
    MCP_CAN &can );


// ****************************************************************************
// Function:    check_for_rx_frame
//
// Purpose:     Checks for CAN frame and stores it if
//              there is one.
//
// Returns:     can_status_t - Status code indicating whether a frame was
//                             received.
//
// Parameters:  [in] can - An MCP_CAN object.
//              [out] frame - A \ref can_frame_s struct in which to store the
//                            frame.
//
// ****************************************************************************
can_status_t check_for_rx_frame(
    MCP_CAN &can,
    can_frame_s * const frame );


#endif /* _OSCC_CAN_H_ */
