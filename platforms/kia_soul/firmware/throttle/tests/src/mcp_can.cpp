#include <stdio.h>
#include "mcp_can.h"

MCP_CAN::MCP_CAN(INT8U _CS) {
    printf("whatever dude\n");
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U rtr, INT8U len, INT8U *buf)
{
    printf("lol we sending a buf\n");
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    printf("lol look another buf here 2\n");
}