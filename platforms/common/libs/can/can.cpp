#include "mcp_can.h"
#include "can.h"
#include "debug.h"

void init_can( MCP_CAN can )
{
    while( can.begin( CAN_BAUD ) != CAN_OK )
    {
        delay( CAN_INIT_RETRY_DELAY );
        DEBUG_PRINTLN( "init_can: retrying" );
    }

    DEBUG_PRINTLN( "init_can: pass" );
}
