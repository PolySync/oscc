#ifndef _MCP2515_H_
#define _MCP2515_H_

#define MAX_CHAR_IN_MESSAGE 8

#include "mcp_can_dfs.h"

class MCP_CAN
{

typedef void (*rust_callback)(uint32_t id, uint8_t len, uint8_t *buf);
public:
    MCP_CAN(INT8U _CS);
    // INT8U begin(INT8U speedset);                                    /* init can                     */
    // INT8U init_Mask(INT8U num, INT8U ext, INT32U ulData);           /* init Masks                   */
    // INT8U init_Filt(INT8U num, INT8U ext, INT32U ulData);           /* init filters                 */
    INT8U sendMsgBuf(INT32U id, INT8U ext, INT8U rtr, INT8U len, INT8U *buf);   /* send buf                     */
    INT8U sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf);   /* send buf                     */
    // INT8U readMsgBuf(INT8U *len, INT8U *buf);                       /* read buf                     */
    // INT8U readMsgBufID(INT32U *ID, INT8U *len, INT8U *buf);         /* read buf with object ID      */
    // INT8U checkReceive(void);                                       /* if something received        */
    // INT8U checkError(void);                                         /* if something error           */
    // INT32U getCanId(void);                                          /* get can id when receive      */
    // INT8U isRemoteRequest(void);                                    /* get RR flag when receive     */
    // INT8U isExtendedFrame(void);                                    /* did we recieve 29bit frame?  */
    int32_t register_callback(rust_callback callback);

private:
    rust_callback cb;
    // INT32U id;
    // INT8U len;
    // INT8U *buf;
};

#endif