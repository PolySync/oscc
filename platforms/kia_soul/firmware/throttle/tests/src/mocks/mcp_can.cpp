#include <stdio.h>
#include "mcp_can.h"

MCP_CAN::MCP_CAN(INT8U _CS) {
}

int32_t MCP_CAN::register_callback(rust_callback callback) {
    cb = callback;
    return 0;
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U rtr, INT8U len, INT8U *buf)
{
    // printf("Sending CAN frame...\n");
    // printf("id: %d\n", id);
    // printf("ext: %d\n", ext);
    // printf("rtr: %d\n", rtr);
    // printf("len: %d\n", len);
    // printf("buf: %d\n", buf);
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    cb(id, len, buf);
}