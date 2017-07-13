#ifndef _OSCC_TEST_MOCK_MCP_CAN_H_
#define _OSCC_TEST_MOCK_MCP_CAN_H_

#include <stdint.h>

#define CAN_500KBPS  15

#ifndef INT32U
#define INT32U unsigned long
#endif

#ifndef INT8U
#define INT8U uint8_t
#endif

#define CAN_OK                  (0)
#define CAN_FAILINIT            (1)
#define CAN_FAILTX              (2)
#define CAN_MSGAVAIL            (3)
#define CAN_NOMSG               (4)
#define CAN_CTRLERROR           (5)
#define CAN_GETTXBFTIMEOUT      (6)
#define CAN_SENDMSGTIMEOUT      (7)
#define CAN_FAIL                (0xff)

class MCP_CAN
{
    public:
        MCP_CAN(INT8U _CS);
        INT8U begin(INT8U speedset);
        INT8U sendMsgBuf(INT32U id, INT8U ext, INT8U len, INT8U *buf);
        INT8U readMsgBufID(INT32U *ID, INT8U *len, INT8U *buf);
        INT8U checkReceive(void);
};

#endif
