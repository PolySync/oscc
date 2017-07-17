/**
 * @file communications.cpp
 *
 */


#include "mcp_can.h"
#include "oscc_can.h"
#include "vehicles/kia_soul.h"

#include "globals.h"
#include "communications.h"


void republish_obd_frames_to_control_can_bus( void )
{
    can_frame_s rx_frame;
    can_status_t ret = check_for_rx_frame( g_obd_can, &rx_frame );

    if( ret == CAN_RX_FRAME_AVAILABLE )
    {
        if( (rx_frame.id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
            || (rx_frame.id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
            || (rx_frame.id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID) )
        {
            g_control_can.sendMsgBuf(
                rx_frame.id,
                CAN_STANDARD,
                sizeof(rx_frame),
                (uint8_t *) &rx_frame );
        }
    }
}
