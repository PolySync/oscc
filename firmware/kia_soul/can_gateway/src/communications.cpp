/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "vehicles/vehicles.h"

#include "globals.h"
#include "communications.h"


void republish_obd_frames_to_control_can_bus( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_obd_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        g_control_can.sendMsgBuf(
            rx_frame.id,
            CAN_STANDARD,
            sizeof(rx_frame),
            (uint8_t *) &rx_frame.data );
    }
}
