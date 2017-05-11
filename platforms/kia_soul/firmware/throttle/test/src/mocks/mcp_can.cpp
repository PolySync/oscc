#include <stdio.h>
#include "mcp_can.h"

MCP_CAN::MCP_CAN(INT8U _CS) {
}

int32_t MCP_CAN::register_callback(rust_callback callback) {
    cb = callback;
    return 0;
}

INT8U MCP_CAN::begin(INT8U speedset) {
    return CAN_OK;
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U rtr, INT8U len, INT8U *buf)
{
    register_can_frame(id, ext, len, buf);
    
    if(cb) {
        cb(id, ext, len, buf);
    }
}

INT8U MCP_CAN::sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf)
{
    register_can_frame(id, ext, len, buf);

    if(cb) {
        cb(id, ext, len, buf);
    }
}

INT8U MCP_CAN::checkReceive(void) {
    if(_id && _len && _buf) {
        return CAN_MSGAVAIL;
    }
    else {
        return CAN_NOMSG;
    }
}  

INT8U MCP_CAN::readMsgBufID(INT32U *ID, INT8U *len, INT8U *buf) {
    *ID = _id;
    *len = _len;

    if(_len > CAN_MAX_CHAR_IN_MESSAGE) {
        memcpy(buf, _buf, CAN_MAX_CHAR_IN_MESSAGE);
    }
    else {
        memcpy(buf, _buf, _len);
    }

    // not sure if needed... do we want message retrieval to only happen once? 
    // Or just as long as it's there
    _id = 0;
    _len = 0;
}

void MCP_CAN::register_can_frame(INT32U id, INT8U ext, INT8U len, INT8U *buf) {
    // store the last recieved message
    _id = id;
    
    if(len > CAN_MAX_CHAR_IN_MESSAGE) {
        len = CAN_MAX_CHAR_IN_MESSAGE;
    }

    _len = len;

    // dest, src, size
    memcpy(_buf, buf, len);
}