#include "mcp_can.h"
#include "debug.h"
#include "time.h"

#include "can.h"


void init_can( MCP_CAN &can )
{
    while( can.begin( CAN_BAUD ) != CAN_OK )
    {
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINTLN( "init_can: retrying" );
    }

    DEBUG_PRINTLN( "init_can: pass" );
}


int check_for_rx_frame( MCP_CAN &can, can_frame_s *frame )
{
    int ret = RX_FRAME_UNAVAILABLE;

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

            ret = RX_FRAME_AVAILABLE;
        }
    }

    return ret;
}
