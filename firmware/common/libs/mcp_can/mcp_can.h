/*
  mcp_can.h
  2012 Copyright (c) Seeed Technology Inc.  All right reserved.

  Author:Loovee (loovee@seeed.cc)
  2014-1-16

  Contributor:

  Cory J. Fowler
  Latonita
  Woodward1
  Mehtajaghvi
  BykeBlast
  TheRo0T
  Tsipizic
  ralfEdmund
  Nathancheek
  BlueAndi
  Adlerweb
  Btetz
  Hurvajs
  ttlappalainen

  The MIT License (MIT)

  Copyright (c) 2013 Seeed Technology Inc.

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#ifndef _MCP2515_H_
#define _MCP2515_H_

#include "mcp_can_dfs.h"

#define MAX_CHAR_IN_MESSAGE 8

class MCP_CAN
{
    private:

    byte   ext_flg;                         // identifier xxxID
                                            // either extended (the 29 LSB) or standard (the 11 LSB)
    unsigned long  can_id;                  // can id
    byte   rtr;                             // rtr
    byte   SPICS;
    SPIClass *pSPI;
    byte   nReservedTx;                     // Count of tx buffers for reserved send

/*
*  mcp2515 driver function
*/

private:

    void mcp2515_reset(void);                                   // reset mcp2515

    byte mcp2515_readRegister(const byte address);              // read mcp2515's register

    void mcp2515_readRegisterS(const byte address,
	                       byte values[],
                               const byte n);
    void mcp2515_setRegister(const byte address,                // set mcp2515's register
                             const byte value);

    void mcp2515_setRegisterS(const byte address,               // set mcp2515's registers
                              const byte values[],
                              const byte n);

    void mcp2515_initCANBuffers(void);

    void mcp2515_modifyRegister(const byte address,             // set bit of one register
                                const byte mask,
                                const byte data);

    byte mcp2515_readStatus(void);                              // read mcp2515's Status
    byte mcp2515_setCANCTRL_Mode(const byte newmode);           // set mode
    byte mcp2515_configRate(const byte canSpeed, const byte clock);  // set baudrate
    byte mcp2515_init(const byte canSpeed, const byte clock);   // mcp2515init

    void mcp2515_write_id( const byte mcp_addr,                 // write can id
                               const byte ext,
                               const unsigned long id );

    void mcp2515_read_id( const byte mcp_addr,                  // read can id
                                    byte* ext,
                                    unsigned long* id );

    void mcp2515_write_canMsg( const byte buffer_sidh_addr, unsigned long id, byte ext, byte rtr, byte len, volatile const byte *buf);     // read can msg
    void mcp2515_read_canMsg( const byte buffer_load_addr, volatile unsigned long *id, volatile byte *ext, volatile byte *rtr, volatile byte *len, volatile byte *buf);   // write can msg
    void mcp2515_start_transmit(const byte mcp_addr);           // start transmit
    byte mcp2515_getNextFreeTXBuf(byte *txbuf_n);               // get Next free txbuf
    byte mcp2515_isTXBufFree(byte *txbuf_n, byte iBuf);         // is buffer by index free

/*
*  can operator function
*/

    byte sendMsg(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent=true); // send message

public:
    MCP_CAN(byte _CS=0);
    void init_CS(byte _CS);                      // define CS after construction before begin()
    void setSPI(SPIClass *_pSPI) { pSPI=_pSPI; } // define SPI port to use before begin()
    void enableTxInterrupt(bool enable=true);    // enable transmit interrupt
    void reserveTxBuffers(byte nTxBuf=0) { nReservedTx=(nTxBuf<MCP_N_TXBUFFERS?nTxBuf:MCP_N_TXBUFFERS-1); }
    byte getLastTxBuffer() { return MCP_N_TXBUFFERS-1; } // read index of last tx buffer

    byte tryTeeMsg( MCP_CAN dest, unsigned long *id, byte *len, byte buf[] );

    byte begin(byte speedset, const byte clockset = MCP_16MHz);     // init can
    byte init_Mask(byte num, byte ext, unsigned long ulData);       // init Masks
    byte init_Filt(byte num, byte ext, unsigned long ulData);       // init filters
    byte sendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent=true);  // send buf
    byte sendMsgBuf(unsigned long id, byte ext, byte len, const byte *buf, bool wait_sent=true);               // send buf
    byte readMsgBuf(byte *len, byte *buf);                          // read buf
    byte readMsgBufID(unsigned long *ID, byte *len, byte *buf);     // read buf with object ID
    byte checkReceive(void);                                        // if something received
    byte checkError(void);                                          // if something error
    unsigned long getCanId(void);                                   // get can id when receive
    byte isRemoteRequest(void);                                     // get RR flag when receive
    byte isExtendedFrame(void);                                     // did we recieve 29bit frame?

    byte readMsgBufID(byte status, volatile unsigned long *id, volatile byte *ext, volatile byte *rtr, volatile byte *len, volatile byte *buf); // read buf with object ID
    byte trySendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, byte iTxBuf=0xff);  // as sendMsgBuf, but does not have any wait for free buffer
    byte sendMsgBuf(byte status, unsigned long id, byte ext, byte rtrBit, byte len, volatile const byte *buf); // send message buf by using parsed buffer status
    inline byte trySendExtMsgBuf(unsigned long id, byte len, const byte *buf, byte iTxBuf=0xff) {  // as trySendMsgBuf, but set ext=1 and rtr=0
      return trySendMsgBuf(id,1,0,len,buf,iTxBuf);
    }
    inline byte sendExtMsgBuf(byte status, unsigned long id, byte len, volatile const byte *buf) { // as sendMsgBuf, but set ext=1 and rtr=0
      return sendMsgBuf(status,id,1,0,len,buf);
    }
    void clearBufferTransmitIfFlags(byte flags=0);                  // Clear transmit flags according to status
    byte readRxTxStatus(void);                                      // read has something send or received
    byte checkClearRxStatus(byte *status);                          // read and clear and return first found rx status bit
    byte checkClearTxStatus(byte *status, byte iTxBuf=0xff);        // read and clear and return first found or buffer specified tx status bit

};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
