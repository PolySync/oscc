#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "mcp_can.h"



INT8U g_mock_mcp_can_check_receive_return;
INT32U g_mock_mcp_can_read_msg_buf_id;
INT8U g_mock_mcp_can_read_msg_buf_buf[8];

INT32U g_mock_mcp_can_send_msg_buf_id;
INT8U g_mock_mcp_can_send_msg_buf_ext;
INT8U g_mock_mcp_can_send_msg_buf_len;
INT8U *g_mock_mcp_can_send_msg_buf_buf;


MCP_CAN::MCP_CAN(INT8U _CS)
{
}

INT8U MCP_CAN::begin(INT8U speedset)
{
    return CAN_OK;
}

INT8U MCP_CAN::checkReceive(void)
{
    return g_mock_mcp_can_check_receive_return;
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    g_mock_mcp_can_send_msg_buf_id = id;
    g_mock_mcp_can_send_msg_buf_ext = ext;
    g_mock_mcp_can_send_msg_buf_len = len;

    g_mock_mcp_can_send_msg_buf_buf = (INT8U *) malloc(len);

    memcpy(g_mock_mcp_can_send_msg_buf_buf, buf, len);

    return CAN_OK;
}

INT8U MCP_CAN::readMsgBufID(INT32U *ID, INT8U *len, INT8U *buf)
{
    *ID = g_mock_mcp_can_read_msg_buf_id;
    *len = 8;

    for( int i = 0; i < *len; ++i )
    {
        buf[i] = g_mock_mcp_can_read_msg_buf_buf[i];
    }

    return CAN_OK;
}
