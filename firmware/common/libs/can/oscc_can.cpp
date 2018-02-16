/**
 * @file oscc_can.cpp
 *
 */


#include <string.h>

#include "debug.h"
#include "mcp_can.h"
#include "oscc_can.h"


void init_can( MCP_CAN &can )
{
    while( can.begin( CAN_BAUD ) != CAN_OK )
    {
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINTLN( "init_can: retrying" );
    }

    DEBUG_PRINTLN( "init_can: pass" );
}


can_status_t check_for_rx_frame( MCP_CAN &can, can_frame_s * const frame )
{
    can_status_t ret = CAN_RX_FRAME_UNKNOWN;

    if( frame != NULL )
    {
        memset( frame, 0, sizeof(*frame) );

        cli();

        int got_message = can.readMsgBufID(
                ( uint32_t* ) &frame->id,
                ( uint8_t* ) &frame->dlc,
                ( uint8_t* ) frame->data );

        if( got_message == CAN_OK )
        {
            frame->timestamp = millis( );

            ret = CAN_RX_FRAME_AVAILABLE;
        }
        else
        {
            ret = CAN_RX_FRAME_UNAVAILABLE;
        }

        sei();
    }

    return ret;
}
