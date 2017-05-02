#include <stdio.h>
#include "mcp_can.h"

MCP_CAN::MCP_CAN(INT8U _CS) {
    printf("whatever dude\n");
}

// int32_t MCP_CAN::register_callback(rust_callback callback) {
//     cb = callback;
//     return 0;
// }

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
    id = id;
    len = len;
    buf = buf;
    // // pass stuff off to rust callback fn for verification
    // printf("Sending CAN frame...\n");
    // printf("id: %d\n", id);
    // printf("ext: %d\n", ext);
    // printf("len: %d\n", len);
    // printf("buf: %d\n", buf);
}