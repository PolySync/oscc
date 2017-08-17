#ifndef _OSCC_TEST_MOCK_MCP_CAN_H_
#define _OSCC_TEST_MOCK_MCP_CAN_H_

#include <stdint.h>

#define CAN_500KBPS  15

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
        MCP_CAN(uint8_t _CS);
        uint8_t begin(uint8_t speedset);
        uint8_t sendMsgBuf(uint32_t id, uint8_t ext, uint8_t len, uint8_t *buf);
        uint8_t readMsgBufID(uint32_t *ID, uint8_t *len, uint8_t *buf);
};

#endif
