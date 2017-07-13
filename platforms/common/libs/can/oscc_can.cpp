#include <string.h>

#include "mcp_can.h"
#include "debug.h"
#include "oscc_time.h"

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
        if( can.checkReceive( ) == CAN_MSGAVAIL )
        {
            memset( frame, 0, sizeof(*frame) );

            frame->timestamp = GET_TIMESTAMP_MS( );

            can.readMsgBufID(
                    ( INT32U* ) &frame->id,
                    ( INT8U* ) &frame->dlc,
                    ( INT8U* ) frame->data );

            ret = CAN_RX_FRAME_AVAILABLE;
        }
        else
        {
            ret = CAN_RX_FRAME_UNAVAILABLE;
        }
    }

    return ret;
}
