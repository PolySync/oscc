#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include "mcp_can.h"



uint8_t g_mock_mcp_can_check_receive_return;
uint32_t g_mock_mcp_can_read_msg_buf_id;
uint8_t g_mock_mcp_can_read_msg_buf_buf[8];

uint32_t g_mock_mcp_can_send_msg_buf_id;
uint8_t g_mock_mcp_can_send_msg_buf_ext;
uint8_t g_mock_mcp_can_send_msg_buf_len;
uint8_t *g_mock_mcp_can_send_msg_buf_buf;


MCP_CAN::MCP_CAN(uint8_t _CS)
{
}

uint8_t MCP_CAN::begin(uint8_t speedset)
{
    return CAN_OK;
}

uint8_t MCP_CAN::sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf)
{
    g_mock_mcp_can_send_msg_buf_id = id;
    g_mock_mcp_can_send_msg_buf_ext = ext;
    g_mock_mcp_can_send_msg_buf_len = len;

    g_mock_mcp_can_send_msg_buf_buf = (uint8_t *) malloc(len);

    memcpy(g_mock_mcp_can_send_msg_buf_buf, buf, len);

    return CAN_OK;
}

uint8_t MCP_CAN::readMsgBufID(uint32_t *ID, uint8_t *len, uint8_t *buf)
{
    if( g_mock_mcp_can_check_receive_return != CAN_MSGAVAIL)
    {
        *len = 0;
        return CAN_NOMSG;
    }

    *ID = g_mock_mcp_can_read_msg_buf_id;
    *len = 8;

    for( int i = 0; i < *len; ++i )
    {
        buf[i] = g_mock_mcp_can_read_msg_buf_buf[i];
    }

    return CAN_OK;
}
