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
    byte   dta_len;                         // data length
    byte   dta[MAX_CHAR_IN_MESSAGE];        // data
    byte   rtr;                             // rtr
    byte   filhit;
    byte   SPICS;

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
    byte mcp2515_configRate(const byte canSpeed);               // set boadrate
    byte mcp2515_init(const byte canSpeed);                     // mcp2515init

    void mcp2515_write_id( const byte mcp_addr,                 // write can id
                               const byte ext,
                               const unsigned long id );

    void mcp2515_read_id( const byte mcp_addr,                  // read can id
                                    byte* ext,
                                    unsigned long* id );

    void mcp2515_write_canMsg( const byte buffer_sidh_addr, int rtrBit );   // write can msg
    void mcp2515_read_canMsg( const byte buffer_sidh_addr);     // read can msg
    void mcp2515_start_transmit(const byte mcp_addr);           // start transmit
    byte mcp2515_getNextFreeTXBuf(byte *txbuf_n);               // get Next free txbuf

/*
*  can operator function
*/

    byte setMsg(unsigned long id, byte ext, byte len, byte rtr, byte *pData);   // set message
    byte setMsg(unsigned long id, byte ext, byte len, byte *pData);             //  set message
    byte clearMsg();                                                // clear all message to zero
    byte readMsg();                                                 // read message
    byte sendMsg(int rtrBit);                                                 // send message

public:
    MCP_CAN(byte _CS);
    byte begin(byte speedset);                                      // init can
    byte init_Mask(byte num, byte ext, unsigned long ulData);       // init Masks
    byte init_Filt(byte num, byte ext, unsigned long ulData);       // init filters
    byte sendMsgBuf(unsigned long id, byte ext, byte rtr, byte len, byte *buf);     // send buf
    byte sendMsgBuf(unsigned long id, byte ext, byte len, byte *buf);               // send buf
    byte readMsgBuf(byte *len, byte *buf);                          // read buf
    byte readMsgBufID(unsigned long *ID, byte *len, byte *buf);     // read buf with object ID
    byte checkReceive(void);                                        // if something received
    byte checkError(void);                                          // if something error
    unsigned long getCanId(void);                                   // get can id when receive
    byte isRemoteRequest(void);                                     // get RR flag when receive
    byte isExtendedFrame(void);                                     // did we recieve 29bit frame?
};

#endif
/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
