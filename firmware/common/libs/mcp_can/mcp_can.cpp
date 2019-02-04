/*
  mcp_can.cpp
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
  xboxpro1
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
#include "mcp_can.h"

#define spi_readwrite      pSPI->transfer
#define spi_read()         spi_readwrite(0x00)
#define spi_write(spi_val) spi_readwrite(spi_val)
#define SPI_BEGIN()        pSPI->beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0))
#define SPI_END()          pSPI->endTransaction()

/*********************************************************************************************************
** Function name:           txCtrlReg
** Descriptions:            return tx ctrl reg according to tx buffer index.
**                          According to my tests this is faster and saves memory compared using vector
*********************************************************************************************************/
byte txCtrlReg(byte i) {
  switch (i) {
    case 0: return MCP_TXB0CTRL;
    case 1: return MCP_TXB1CTRL;
    case 2: return MCP_TXB2CTRL;
  }
  return MCP_TXB2CTRL;
}

/*********************************************************************************************************
** Function name:           statusToBuffer
** Descriptions:            converts CANINTF status to tx buffer index
*********************************************************************************************************/
byte statusToTxBuffer(byte status)
{
  switch ( status ) {
    case MCP_TX0IF : return 0;
    case MCP_TX1IF : return 1;
    case MCP_TX2IF : return 2;
  }

  return 0xff;
}

/*********************************************************************************************************
** Function name:           statusToBuffer
** Descriptions:            converts CANINTF status to tx buffer sidh
*********************************************************************************************************/
byte statusToTxSidh(byte status)
{
  switch ( status ) {
    case MCP_TX0IF : return MCP_TXB0SIDH;
    case MCP_TX1IF : return MCP_TXB1SIDH;
    case MCP_TX2IF : return MCP_TXB2SIDH;
  }

  return 0;
}

/*********************************************************************************************************
** Function name:           txSidhToTxLoad
** Descriptions:            return tx load command according to tx buffer sidh register
*********************************************************************************************************/
byte txSidhToRTS(byte sidh) {
  switch (sidh) {
    case MCP_TXB0SIDH: return MCP_RTS_TX0;
    case MCP_TXB1SIDH: return MCP_RTS_TX1;
    case MCP_TXB2SIDH: return MCP_RTS_TX2;
  }
  return 0;
}

/*********************************************************************************************************
** Function name:           txSidhToTxLoad
** Descriptions:            return tx load command according to tx buffer sidh register
*********************************************************************************************************/
byte txSidhToTxLoad(byte sidh) {
  switch (sidh) {
    case MCP_TXB0SIDH: return MCP_LOAD_TX0;
    case MCP_TXB1SIDH: return MCP_LOAD_TX1;
    case MCP_TXB2SIDH: return MCP_LOAD_TX2;
  }
  return 0;
}

/*********************************************************************************************************
** Function name:           txIfFlag
** Descriptions:            return tx interrupt flag
*********************************************************************************************************/
byte txIfFlag(byte i) {
  switch (i) {
    case 0: return MCP_TX0IF;
    case 1: return MCP_TX1IF;
    case 2: return MCP_TX2IF;
  }
  return 0;
}

/*********************************************************************************************************
** Function name:           txStatusPendingFlag
** Descriptions:            return buffer tx pending flag on status
*********************************************************************************************************/
byte txStatusPendingFlag(byte i) {
  switch (i) {
    case 0: return MCP_STAT_TX0_PENDING;
    case 1: return MCP_STAT_TX1_PENDING;
    case 2: return MCP_STAT_TX2_PENDING;
  }
  return 0xff;
}

/*********************************************************************************************************
** Function name:           mcp2515_reset
** Descriptions:            reset the device
*********************************************************************************************************/
void MCP_CAN::mcp2515_reset(void)
{
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_RESET);
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
    delay(10);
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegister
** Descriptions:            read register
*********************************************************************************************************/
byte MCP_CAN::mcp2515_readRegister(const byte address)
{
    byte ret;

#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_READ);
    spi_readwrite(address);
    ret = spi_read();
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif

    return ret;
}

/*********************************************************************************************************
** Function name:           mcp2515_readRegisterS
** Descriptions:            read registerS
*********************************************************************************************************/
void MCP_CAN::mcp2515_readRegisterS(const byte address, byte values[], const byte n)
{
    byte i;
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_READ);
    spi_readwrite(address);
    // mcp2515 has auto-increment of address-pointer
    for (i=0; i<n && i<CAN_MAX_CHAR_IN_MESSAGE; i++)
    {
      values[i] = spi_read();
    }
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegister
** Descriptions:            set register
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegister(const byte address, const byte value)
{
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);
    spi_readwrite(value);
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
}

/*********************************************************************************************************
** Function name:           mcp2515_setRegisterS
** Descriptions:            set registerS
*********************************************************************************************************/
void MCP_CAN::mcp2515_setRegisterS(const byte address, const byte values[], const byte n)
{
    byte i;
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_WRITE);
    spi_readwrite(address);

    for (i=0; i<n; i++)
    {
      spi_readwrite(values[i]);
    }
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
}

/*********************************************************************************************************
** Function name:           mcp2515_modifyRegister
** Descriptions:            set bit of one register
*********************************************************************************************************/
void MCP_CAN::mcp2515_modifyRegister(const byte address, const byte mask, const byte data)
{
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_BITMOD);
    spi_readwrite(address);
    spi_readwrite(mask);
    spi_readwrite(data);
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
}

/*********************************************************************************************************
** Function name:           mcp2515_readStatus
** Descriptions:            read mcp2515's Status
*********************************************************************************************************/
byte MCP_CAN::mcp2515_readStatus(void)
{
    byte i;
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
    MCP2515_SELECT();
    spi_readwrite(MCP_READ_STATUS);
    i = spi_read();
    MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif

    return i;
}

/*********************************************************************************************************
** Function name:           mcp2515_setCANCTRL_Mode
** Descriptions:            set control mode
*********************************************************************************************************/
byte MCP_CAN::mcp2515_setCANCTRL_Mode(const byte newmode)
{
    byte i;

    mcp2515_modifyRegister(MCP_CANCTRL, MODE_MASK, newmode);

    i = mcp2515_readRegister(MCP_CANCTRL);
    i &= MODE_MASK;

    if ( i == newmode )
    {
      return MCP2515_OK;
    }

    return MCP2515_FAIL;
}

/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            set baudrate
*********************************************************************************************************/
byte MCP_CAN::mcp2515_configRate(const byte canSpeed, const byte clock)
{
  byte set, cfg1, cfg2, cfg3;
  set = 1;
  switch (clock)
  {
    case (MCP_16MHz) :
      switch (canSpeed)
      {
        case (CAN_5KBPS):
          cfg1 = MCP_16MHz_5kBPS_CFG1;
          cfg2 = MCP_16MHz_5kBPS_CFG2;
          cfg3 = MCP_16MHz_5kBPS_CFG3;
          break;

        case (CAN_10KBPS):
          cfg1 = MCP_16MHz_10kBPS_CFG1;
          cfg2 = MCP_16MHz_10kBPS_CFG2;
          cfg3 = MCP_16MHz_10kBPS_CFG3;
          break;

        case (CAN_20KBPS):
          cfg1 = MCP_16MHz_20kBPS_CFG1;
          cfg2 = MCP_16MHz_20kBPS_CFG2;
          cfg3 = MCP_16MHz_20kBPS_CFG3;
          break;

        case (CAN_25KBPS):
          cfg1 = MCP_16MHz_25kBPS_CFG1;
          cfg2 = MCP_16MHz_25kBPS_CFG2;
          cfg3 = MCP_16MHz_25kBPS_CFG3;
          break;

        case (CAN_31K25BPS):
          cfg1 = MCP_16MHz_31k25BPS_CFG1;
          cfg2 = MCP_16MHz_31k25BPS_CFG2;
          cfg3 = MCP_16MHz_31k25BPS_CFG3;
          break;

        case (CAN_33KBPS):
          cfg1 = MCP_16MHz_33kBPS_CFG1;
          cfg2 = MCP_16MHz_33kBPS_CFG2;
          cfg3 = MCP_16MHz_33kBPS_CFG3;
          break;

        case (CAN_40KBPS):
          cfg1 = MCP_16MHz_40kBPS_CFG1;
          cfg2 = MCP_16MHz_40kBPS_CFG2;
          cfg3 = MCP_16MHz_40kBPS_CFG3;
          break;

        case (CAN_50KBPS):
          cfg1 = MCP_16MHz_50kBPS_CFG1;
          cfg2 = MCP_16MHz_50kBPS_CFG2;
          cfg3 = MCP_16MHz_50kBPS_CFG3;
          break;

        case (CAN_80KBPS):
          cfg1 = MCP_16MHz_80kBPS_CFG1;
          cfg2 = MCP_16MHz_80kBPS_CFG2;
          cfg3 = MCP_16MHz_80kBPS_CFG3;
          break;

        case (CAN_83K3BPS):
          cfg1 = MCP_16MHz_83k3BPS_CFG1;
          cfg2 = MCP_16MHz_83k3BPS_CFG2;
          cfg3 = MCP_16MHz_83k3BPS_CFG3;
          break;

        case (CAN_95KBPS):
          cfg1 = MCP_16MHz_95kBPS_CFG1;
          cfg2 = MCP_16MHz_95kBPS_CFG2;
          cfg3 = MCP_16MHz_95kBPS_CFG3;
          break;

        case (CAN_100KBPS):
          cfg1 = MCP_16MHz_100kBPS_CFG1;
          cfg2 = MCP_16MHz_100kBPS_CFG2;
          cfg3 = MCP_16MHz_100kBPS_CFG3;
          break;

        case (CAN_125KBPS):
          cfg1 = MCP_16MHz_125kBPS_CFG1;
          cfg2 = MCP_16MHz_125kBPS_CFG2;
          cfg3 = MCP_16MHz_125kBPS_CFG3;
          break;

        case (CAN_200KBPS):
          cfg1 = MCP_16MHz_200kBPS_CFG1;
          cfg2 = MCP_16MHz_200kBPS_CFG2;
          cfg3 = MCP_16MHz_200kBPS_CFG3;
          break;

        case (CAN_250KBPS):
          cfg1 = MCP_16MHz_250kBPS_CFG1;
          cfg2 = MCP_16MHz_250kBPS_CFG2;
          cfg3 = MCP_16MHz_250kBPS_CFG3;
          break;

        case (CAN_500KBPS):
          cfg1 = MCP_16MHz_500kBPS_CFG1;
          cfg2 = MCP_16MHz_500kBPS_CFG2;
          cfg3 = MCP_16MHz_500kBPS_CFG3;
          break;

        case (CAN_666KBPS):
          cfg1 = MCP_16MHz_666kBPS_CFG1;
          cfg2 = MCP_16MHz_666kBPS_CFG2;
          cfg3 = MCP_16MHz_666kBPS_CFG3;
          break;

        case (CAN_1000KBPS):
          cfg1 = MCP_16MHz_1000kBPS_CFG1;
          cfg2 = MCP_16MHz_1000kBPS_CFG2;
          cfg3 = MCP_16MHz_1000kBPS_CFG3;
          break;

        default:
          set = 0;
          break;
      }
      break;

    case (MCP_8MHz) :
      switch (canSpeed)
      {
        case (CAN_5KBPS) :
          cfg1 = MCP_8MHz_5kBPS_CFG1;
          cfg2 = MCP_8MHz_5kBPS_CFG2;
          cfg3 = MCP_8MHz_5kBPS_CFG3;
          break;

        case (CAN_10KBPS) :
          cfg1 = MCP_8MHz_10kBPS_CFG1;
          cfg2 = MCP_8MHz_10kBPS_CFG2;
          cfg3 = MCP_8MHz_10kBPS_CFG3;
          break;

        case (CAN_20KBPS) :
          cfg1 = MCP_8MHz_20kBPS_CFG1;
          cfg2 = MCP_8MHz_20kBPS_CFG2;
          cfg3 = MCP_8MHz_20kBPS_CFG3;
          break;

        case (CAN_31K25BPS) :
          cfg1 = MCP_8MHz_31k25BPS_CFG1;
          cfg2 = MCP_8MHz_31k25BPS_CFG2;
          cfg3 = MCP_8MHz_31k25BPS_CFG3;
          break;

        case (CAN_40KBPS) :
          cfg1 = MCP_8MHz_40kBPS_CFG1;
          cfg2 = MCP_8MHz_40kBPS_CFG2;
          cfg3 = MCP_8MHz_40kBPS_CFG3;
          break;

        case (CAN_50KBPS) :
          cfg1 = MCP_8MHz_50kBPS_CFG1;
          cfg2 = MCP_8MHz_50kBPS_CFG2;
          cfg3 = MCP_8MHz_50kBPS_CFG3;
          break;

        case (CAN_80KBPS) :
          cfg1 = MCP_8MHz_80kBPS_CFG1;
          cfg2 = MCP_8MHz_80kBPS_CFG2;
          cfg3 = MCP_8MHz_80kBPS_CFG3;
          break;

        case (CAN_100KBPS) :
          cfg1 = MCP_8MHz_100kBPS_CFG1;
          cfg2 = MCP_8MHz_100kBPS_CFG2;
          cfg3 = MCP_8MHz_100kBPS_CFG3;
          break;

        case (CAN_125KBPS) :
          cfg1 = MCP_8MHz_125kBPS_CFG1;
          cfg2 = MCP_8MHz_125kBPS_CFG2;
          cfg3 = MCP_8MHz_125kBPS_CFG3;
          break;

        case (CAN_200KBPS) :
          cfg1 = MCP_8MHz_200kBPS_CFG1;
          cfg2 = MCP_8MHz_200kBPS_CFG2;
          cfg3 = MCP_8MHz_200kBPS_CFG3;
          break;

        case (CAN_250KBPS) :
          cfg1 = MCP_8MHz_250kBPS_CFG1;
          cfg2 = MCP_8MHz_250kBPS_CFG2;
          cfg3 = MCP_8MHz_250kBPS_CFG3;
          break;

        case (CAN_500KBPS) :
          cfg1 = MCP_8MHz_500kBPS_CFG1;
          cfg2 = MCP_8MHz_500kBPS_CFG2;
          cfg3 = MCP_8MHz_500kBPS_CFG3;
          break;

        case (CAN_1000KBPS) :
          cfg1 = MCP_8MHz_1000kBPS_CFG1;
          cfg2 = MCP_8MHz_1000kBPS_CFG2;
          cfg3 = MCP_8MHz_1000kBPS_CFG3;
          break;

        default:
          set = 0;
          break;
      }
      break;

    default:
      set = 0;
      break;
  }

  if (set) {
    mcp2515_setRegister(MCP_CNF1, cfg1);
    mcp2515_setRegister(MCP_CNF2, cfg2);
    mcp2515_setRegister(MCP_CNF3, cfg3);
    return MCP2515_OK;
  }
  else {
    return MCP2515_FAIL;
  }
}

/*********************************************************************************************************
** Function name:           mcp2515_initCANBuffers
** Descriptions:            init canbuffers
*********************************************************************************************************/
void MCP_CAN::mcp2515_initCANBuffers(void)
{
    byte i, a1, a2, a3;

    a1 = MCP_TXB0CTRL;
    a2 = MCP_TXB1CTRL;
    a3 = MCP_TXB2CTRL;
    for (i = 0; i < 14; i++)                         // in-buffer loop
    {
      mcp2515_setRegister(a1, 0);
      mcp2515_setRegister(a2, 0);
      mcp2515_setRegister(a3, 0);
      a1++;
      a2++;
      a3++;
    }
    mcp2515_setRegister(MCP_RXB0CTRL, 0);
    mcp2515_setRegister(MCP_RXB1CTRL, 0);
}

/*********************************************************************************************************
** Function name:           mcp2515_init
** Descriptions:            init the device
*********************************************************************************************************/
byte MCP_CAN::mcp2515_init(const byte canSpeed, const byte clock)
{

    byte res;

    mcp2515_reset();

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0)
    {
#if DEBUG_EN
      Serial.print("Enter setting mode fail\r\n");
#else
      delay(10);
#endif
      return res;
    }
#if DEBUG_EN
    Serial.print("Enter setting mode success \r\n");
#else
    delay(10);
#endif

    // set boadrate
    if (mcp2515_configRate(canSpeed, clock))
    {
#if DEBUG_EN
      Serial.print("set rate fall!!\r\n");
#else
      delay(10);
#endif
      return res;
    }
#if DEBUG_EN
    Serial.print("set rate success!!\r\n");
#else
    delay(10);
#endif

    if ( res == MCP2515_OK ) {

      // init canbuffers
      mcp2515_initCANBuffers();

      // interrupt mode
      mcp2515_setRegister(MCP_CANINTE, MCP_RX0IF | MCP_RX1IF);

#if (DEBUG_RXANY==1)
      // enable both receive-buffers to receive any message and enable rollover
      mcp2515_modifyRegister(MCP_RXB0CTRL,
                             MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
                             MCP_RXB_RX_ANY | MCP_RXB_BUKT_MASK);
      mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
                             MCP_RXB_RX_ANY);
#else
      // enable both receive-buffers to receive messages with std. and ext. identifiers and enable rollover
      mcp2515_modifyRegister(MCP_RXB0CTRL,
                             MCP_RXB_RX_MASK | MCP_RXB_BUKT_MASK,
                             MCP_RXB_RX_STDEXT | MCP_RXB_BUKT_MASK);
      mcp2515_modifyRegister(MCP_RXB1CTRL, MCP_RXB_RX_MASK,
                             MCP_RXB_RX_STDEXT);
#endif
      // enter normal mode
      res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
      if (res)
      {
#if DEBUG_EN
        Serial.print("Enter Normal Mode Fail!!\r\n");
#else
        delay(10);
#endif
        return res;
      }


#if DEBUG_EN
      Serial.print("Enter Normal Mode Success!!\r\n");
#else
      delay(10);
#endif

    }
    return res;

}

/*********************************************************************************************************
** Function name:           mcp2515_id_to_buf
** Descriptions:            configure tbufdata[4] from id and ext
*********************************************************************************************************/
void mcp2515_id_to_buf(const byte ext, const unsigned long id, byte *tbufdata)
{
  uint16_t canid;

  canid = (uint16_t)(id & 0x0FFFF);

  if ( ext == 1)
  {
    tbufdata[MCP_EID0] = (byte) (canid & 0xFF);
    tbufdata[MCP_EID8] = (byte) (canid >> 8);
    canid = (uint16_t)(id >> 16);
    tbufdata[MCP_SIDL] = (byte) (canid & 0x03);
    tbufdata[MCP_SIDL] += (byte) ((canid & 0x1C) << 3);
    tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
    tbufdata[MCP_SIDH] = (byte) (canid >> 5 );
  }
  else
  {
    tbufdata[MCP_SIDH] = (byte) (canid >> 3 );
    tbufdata[MCP_SIDL] = (byte) ((canid & 0x07 ) << 5);
    tbufdata[MCP_EID0] = 0;
    tbufdata[MCP_EID8] = 0;
  }
}

/*********************************************************************************************************
** Function name:           mcp2515_write_id
** Descriptions:            write can id
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_id(const byte mcp_addr, const byte ext, const unsigned long id)
{
    byte tbufdata[4];

    mcp2515_id_to_buf(ext,id,tbufdata);
    mcp2515_setRegisterS(mcp_addr, tbufdata, 4);
}

/*********************************************************************************************************
** Function name:           mcp2515_read_id
** Descriptions:            read can id
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_id(const byte mcp_addr, byte* ext, unsigned long* id)
{
    byte tbufdata[4];

    *ext    = 0;
    *id     = 0;

    mcp2515_readRegisterS(mcp_addr, tbufdata, 4);

    *id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);

    if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M )
    {
      // extended id
      *id = (*id << 2) + (tbufdata[MCP_SIDL] & 0x03);
      *id = (*id << 8) + tbufdata[MCP_EID8];
      *id = (*id << 8) + tbufdata[MCP_EID0];
      *ext = 1;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_write_canMsg
** Descriptions:            write msg
**                          Note! There is no check for right address!
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_canMsg(const byte buffer_sidh_addr, unsigned long id, byte ext, byte rtrBit, byte len, volatile const byte *buf)
{
  byte load_addr=txSidhToTxLoad(buffer_sidh_addr);

  byte tbufdata[4];
  byte dlc = len | ( rtrBit ? MCP_RTR_MASK : 0 ) ;
  byte i;

  mcp2515_id_to_buf(ext,id,tbufdata);

#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
  MCP2515_SELECT();
  spi_readwrite(load_addr);
  for (i = 0; i < 4; i++) spi_write(tbufdata[i]);
  spi_write(dlc);
  for (i = 0; i < len && i<CAN_MAX_CHAR_IN_MESSAGE; i++) spi_write(buf[i]);

  MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif

  mcp2515_start_transmit( buffer_sidh_addr );

}

/*********************************************************************************************************
** Function name:           mcp2515_read_canMsg
** Descriptions:            read message
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_canMsg( const byte buffer_load_addr, volatile unsigned long *id, volatile byte *ext, volatile byte *rtrBit, volatile byte *len, volatile byte *buf)        /* read can msg                 */
{
  byte tbufdata[4];
  byte i;

  MCP2515_SELECT();
  spi_readwrite(buffer_load_addr);
  // mcp2515 has auto-increment of address-pointer
  for (i = 0; i < 4; i++) tbufdata[i] = spi_read();

  *id = (tbufdata[MCP_SIDH] << 3) + (tbufdata[MCP_SIDL] >> 5);
  *ext = 0;
  if ( (tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M )
  {
    /* extended id                  */
    *id = (*id << 2) + (tbufdata[MCP_SIDL] & 0x03);
    *id = (*id << 8) + tbufdata[MCP_EID8];
    *id = (*id << 8) + tbufdata[MCP_EID0];
    *ext = 1;
  }

  byte pMsgSize = spi_read();
  *len = pMsgSize & MCP_DLC_MASK;
  *rtrBit = (pMsgSize & MCP_RTR_MASK) ? 1 : 0;
  for (i = 0; i < *len && i<CAN_MAX_CHAR_IN_MESSAGE; i++) {
    buf[i] = spi_read();
  }

  MCP2515_UNSELECT();
}

/*********************************************************************************************************
** Function name:           mcp2515_start_transmit
** Descriptions:            Start message transmit on mcp2515
*********************************************************************************************************/
void MCP_CAN::mcp2515_start_transmit(const byte mcp_addr)              // start transmit
{
#ifdef SPI_HAS_TRANSACTION
    SPI_BEGIN();
#endif
  MCP2515_SELECT();
  spi_readwrite(txSidhToRTS(mcp_addr));
  MCP2515_UNSELECT();
#ifdef SPI_HAS_TRANSACTION
    SPI_END();
#endif
}

/*********************************************************************************************************
** Function name:           mcp2515_isTXBufFree
** Descriptions:            Test is tx buffer free for transmitting
*********************************************************************************************************/
byte MCP_CAN::mcp2515_isTXBufFree(byte *txbuf_n, byte iBuf)           /* get Next free txbuf          */
{
  *txbuf_n = 0x00;

  if ( iBuf>=MCP_N_TXBUFFERS ||
      (mcp2515_readStatus() & txStatusPendingFlag(iBuf))!=0 ) return MCP_ALLTXBUSY;

  *txbuf_n = txCtrlReg(iBuf) + 1;                                /* return SIDH-address of Buffer */
  mcp2515_modifyRegister(MCP_CANINTF, txIfFlag(iBuf), 0);

  return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           mcp2515_getNextFreeTXBuf
** Descriptions:            finds next free tx buffer for sending. Return MCP_ALLTXBUSY, if there is none.
*********************************************************************************************************/
byte MCP_CAN::mcp2515_getNextFreeTXBuf(byte *txbuf_n)                 // get Next free txbuf
{
    byte status=mcp2515_readStatus() & MCP_STAT_TX_PENDING_MASK;
    byte i;

    *txbuf_n = 0x00;

    if ( status==MCP_STAT_TX_PENDING_MASK ) return MCP_ALLTXBUSY; // All buffers are pending

    // check all 3 TX-Buffers except reserved
    for (i = 0; i < MCP_N_TXBUFFERS-nReservedTx; i++)
    {
      if ( (status & txStatusPendingFlag(i) ) == 0 ) {
        *txbuf_n = txCtrlReg(i) + 1;                                   // return SIDH-address of Buffer
        mcp2515_modifyRegister(MCP_CANINTF, txIfFlag(i), 0);
        return MCP2515_OK;                                                 // ! function exit
      }
    }

    return MCP_ALLTXBUSY;
}

/*********************************************************************************************************
** Function name:           MCP_CAN
** Descriptions:            Constructor
*********************************************************************************************************/
MCP_CAN::MCP_CAN(byte _CS) : nReservedTx(0)
{
  pSPI=&SPI; init_CS(_CS);
}

/*********************************************************************************************************
** Function name:           set CS
** Descriptions:            init CS pin and set UNSELECTED
*********************************************************************************************************/
void MCP_CAN::init_CS(byte _CS)
{
  if (_CS == 0) return;
  SPICS = _CS;
  pinMode(SPICS, OUTPUT);
  MCP2515_UNSELECT();
}

/*********************************************************************************************************
** Function name:           begin
** Descriptions:            init can and set speed
*********************************************************************************************************/
byte MCP_CAN::begin(byte speedset, const byte clockset)
{
    pSPI->begin();
    byte res = mcp2515_init(speedset, clockset);
    return ((res == MCP2515_OK) ? CAN_OK : CAN_FAILINIT);
}

/*********************************************************************************************************
** Function name:           enableTxInterrupt
** Descriptions:            enable interrupt for all tx buffers
*********************************************************************************************************/
void MCP_CAN::enableTxInterrupt(bool enable)
{
  byte interruptStatus=mcp2515_readRegister(MCP_CANINTE);

  if ( enable ) {
    interruptStatus |= MCP_TX_INT;
  } else {
    interruptStatus &= ~MCP_TX_INT;
  }

  mcp2515_setRegister(MCP_CANINTE, interruptStatus);
}

/*********************************************************************************************************
** Function name:           init_Mask
** Descriptions:            init canid Masks
*********************************************************************************************************/
byte MCP_CAN::init_Mask(byte num, byte ext, unsigned long ulData)
{
    byte res = MCP2515_OK;
#if DEBUG_EN
    Serial.print("Begin to set Mask!!\r\n");
#else
    delay(10);
#endif
    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0) {
#if DEBUG_EN
        Serial.print("Enter setting mode fall\r\n");
#else
        delay(10);
#endif
        return res;
    }

    if (num == 0) {
        mcp2515_write_id(MCP_RXM0SIDH, ext, ulData);

    }
    else if (num == 1) {
        mcp2515_write_id(MCP_RXM1SIDH, ext, ulData);
    }
    else res =  MCP2515_FAIL;

    res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
    if (res > 0) {
#if DEBUG_EN
        Serial.print("Enter normal mode fall\r\n");
#else
        delay(10);
#endif
        return res;
    }
#if DEBUG_EN
    Serial.print("set Mask success!!\r\n");
#else
    delay(10);
#endif
    return res;
}

/*********************************************************************************************************
** Function name:           init_Filt
** Descriptions:            init canid filters
*********************************************************************************************************/
byte MCP_CAN::init_Filt(byte num, byte ext, unsigned long ulData)
{
    byte res = MCP2515_OK;
#if DEBUG_EN
    Serial.print("Begin to set Filter!!\r\n");
#else
    delay(10);
#endif
    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if (res > 0)
    {
#if DEBUG_EN
        Serial.print("Enter setting mode fall\r\n");
#else
        delay(10);
#endif
        return res;
    }

    switch ( num )
    {
      case 0:
        mcp2515_write_id(MCP_RXF0SIDH, ext, ulData);
        break;

      case 1:
        mcp2515_write_id(MCP_RXF1SIDH, ext, ulData);
        break;

      case 2:
        mcp2515_write_id(MCP_RXF2SIDH, ext, ulData);
        break;

      case 3:
        mcp2515_write_id(MCP_RXF3SIDH, ext, ulData);
        break;

      case 4:
        mcp2515_write_id(MCP_RXF4SIDH, ext, ulData);
        break;

      case 5:
        mcp2515_write_id(MCP_RXF5SIDH, ext, ulData);
        break;

      default:
        res = MCP2515_FAIL;
    }

    res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
    if (res > 0)
    {
#if DEBUG_EN
        Serial.print("Enter normal mode fall\r\nSet filter fail!!\r\n");
#else
        delay(10);
#endif
        return res;
    }
#if DEBUG_EN
    Serial.print("set Filter success!!\r\n");
#else
    delay(10);
#endif

    return res;
}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            Send message by using buffer read as free from CANINTF status
**                          Status has to be read with readRxTxStatus and filtered with checkClearTxStatus
*********************************************************************************************************/
byte MCP_CAN::sendMsgBuf(byte status, unsigned long id, byte ext, byte rtrBit, byte len, volatile const byte *buf)
{
  byte txbuf_n=statusToTxSidh(status);

  if ( txbuf_n==0 ) return CAN_FAILTX; // Invalid status

  mcp2515_modifyRegister(MCP_CANINTF, status, 0);  // Clear interrupt flag
  mcp2515_write_canMsg(txbuf_n, id, ext, rtrBit, len, buf);

  return CAN_OK;
}

/* ******************************************************************************************************
 * tryTeeMsg attempts to copy a message from one CAN bus to another as quickly as possible, while 
 * allowing the caller to provide storage for message details.
 * ******************************************************************************************************/
byte MCP_CAN::tryTeeMsg( MCP_CAN dest, unsigned long *id, byte *len, byte buf[] )
{
  byte rx_ext;
  byte rx_rtr;
  byte tx_buf_n;
  
  byte src_status = mcp2515_readStatus();

  // if no message is available to be read, return CAN_NOMSG immediately
  if ( !(src_status & MCP_STAT_RXIF_MASK) ) {
    return CAN_NOMSG;
  }

  // read the message from the src bus
  byte read_status = CAN_NOMSG;
  if ( src_status & MCP_RX0IF ) // Msg in Buffer 0
  {
    mcp2515_read_canMsg( MCP_READ_RX0, id, &rx_ext, &rx_rtr, len, buf);
    read_status = CAN_OK;
  }
  else if ( src_status & MCP_RX1IF ) // Msg in Buffer 1
  {
    mcp2515_read_canMsg( MCP_READ_RX1, id, &rx_ext, &rx_rtr, len, buf);
    read_status = CAN_OK;
  }

  // return CAN_NOMSG if the read failed
  if (read_status != CAN_OK) {
    return CAN_NOMSG;
  }

  // attempt to grab a free transmit buffer from the destination bus
  if ( dest.mcp2515_getNextFreeTXBuf(&tx_buf_n) != MCP2515_OK ) {
    return CAN_GETTXBFTIMEOUT;
  }

  // write the message into the transmit buffer
  dest.mcp2515_write_canMsg(tx_buf_n, *id, rx_ext, rx_rtr, *len, buf);
  
  return CAN_MSGAVAIL;
}

/*********************************************************************************************************
** Function name:           trySendMsgBuf
** Descriptions:            Try to send message. There is no delays for waiting free buffer.
*********************************************************************************************************/
byte MCP_CAN::trySendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, byte iTxBuf)
{
  byte txbuf_n;

  if ( iTxBuf<MCP_N_TXBUFFERS ) { // Use specified buffer
    if ( mcp2515_isTXBufFree(&txbuf_n,iTxBuf) != MCP2515_OK ) return CAN_FAILTX;
  } else {
    if ( mcp2515_getNextFreeTXBuf(&txbuf_n) != MCP2515_OK ) return CAN_FAILTX;
  }

  mcp2515_write_canMsg(txbuf_n, id, ext, rtrBit, len, buf);

  return CAN_OK;
}

/*********************************************************************************************************
** Function name:           sendMsg
** Descriptions:            send message
*********************************************************************************************************/
byte MCP_CAN::sendMsg(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent)
{
    byte res, res1, txbuf_n;
    uint16_t uiTimeOut = 0;

    can_id=id;
    ext_flg=ext;
    rtr=rtrBit;

    do {
        if (uiTimeOut > 0) delayMicroseconds(10);
        res = mcp2515_getNextFreeTXBuf(&txbuf_n);                       // info = addr.
        uiTimeOut++;
    } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

    if (uiTimeOut == TIMEOUTVALUE)
    {
        return CAN_GETTXBFTIMEOUT;                                      // get tx buff time out
    }
    mcp2515_write_canMsg(txbuf_n, id, ext, rtrBit, len, buf);

    if (wait_sent) {
      uiTimeOut = 0;
      do
      {
        if (uiTimeOut > 0) delayMicroseconds(10);
          uiTimeOut++;
          res1 = mcp2515_readRegister(txbuf_n - 1);  // read send buff ctrl reg
          res1 = res1 & 0x08;
      } while (res1 && (uiTimeOut < TIMEOUTVALUE));

      if (uiTimeOut == TIMEOUTVALUE)                                       // send msg timeout
      {
          return CAN_SENDMSGTIMEOUT;
      }
    }

    return CAN_OK;

}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            send buf
*********************************************************************************************************/
byte MCP_CAN::sendMsgBuf(unsigned long id, byte ext, byte rtrBit, byte len, const byte *buf, bool wait_sent)
{
    return sendMsg(id,ext,rtrBit,len,buf,wait_sent);
}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            send buf
*********************************************************************************************************/
byte MCP_CAN::sendMsgBuf(unsigned long id, byte ext, byte len, const byte *buf, bool wait_sent)
{
    return sendMsg(id,ext,0,len,buf,wait_sent);
}


/*********************************************************************************************************
** Function name:           readMsgBuf
** Descriptions:            read message buf
*********************************************************************************************************/
byte MCP_CAN::readMsgBuf(byte *len, byte buf[])
{
    return readMsgBufID(readRxTxStatus(),&can_id,&ext_flg,&rtr,len,buf);
}

/*********************************************************************************************************
** Function name:           readMsgBufID
** Descriptions:            read message buf and can bus source ID
*********************************************************************************************************/
byte MCP_CAN::readMsgBufID(unsigned long *ID, byte *len, byte buf[])
{
    return readMsgBufID(readRxTxStatus(),ID,&ext_flg,&rtr,len,buf);
}

/*********************************************************************************************************
** Function name:           readMsgBufID
** Descriptions:            Read message buf and can bus source ID according to status.
**                          Status has to be read with readRxTxStatus.
*********************************************************************************************************/
byte MCP_CAN::readMsgBufID(byte status, volatile unsigned long *id, volatile byte *ext, volatile byte *rtrBit, volatile byte *len, volatile byte *buf)
{
  byte rc=CAN_NOMSG;

  if ( status & MCP_RX0IF )                                        // Msg in Buffer 0
  {
    mcp2515_read_canMsg( MCP_READ_RX0, id, ext, rtrBit, len, buf);
    rc = CAN_OK;
  }
  else if ( status & MCP_RX1IF )                                   // Msg in Buffer 1
  {
    mcp2515_read_canMsg( MCP_READ_RX1, id, ext, rtrBit, len, buf);
    rc = CAN_OK;
  }

  if (rc==CAN_OK) {
    rtr=*rtrBit;
    // dta_len=*len; // not used on any interface function
    ext_flg=*ext;
    can_id=*id;
  } else {
    *len=0;
  }

  return rc;
}

/*********************************************************************************************************
** Function name:           readRxTxStatus
** Descriptions:            Read RX and TX interrupt bits. Function uses status reading, but translates.
**                          result to MCP_CANINTF. With this you can check status e.g. on interrupt sr
**                          with one single call to save SPI calls. Then use checkClearRxStatus and
**                          checkClearTxStatus for testing.
*********************************************************************************************************/
byte MCP_CAN::readRxTxStatus(void)
{
  byte ret=( mcp2515_readStatus() & ( MCP_STAT_TXIF_MASK | MCP_STAT_RXIF_MASK ) );
  ret=(ret & MCP_STAT_TX0IF ? MCP_TX0IF : 0) |
      (ret & MCP_STAT_TX1IF ? MCP_TX1IF : 0) |
      (ret & MCP_STAT_TX2IF ? MCP_TX2IF : 0) |
      (ret & MCP_STAT_RXIF_MASK); // Rx bits happend to be same on status and MCP_CANINTF
  return ret;
}

/*********************************************************************************************************
** Function name:           checkClearRxStatus
** Descriptions:            Return first found rx CANINTF status and clears it from parameter.
**                          Note that this does not affect to chip CANINTF at all. You can use this
**                          with one single readRxTxStatus call.
*********************************************************************************************************/
byte MCP_CAN::checkClearRxStatus(byte *status)
{
  byte ret;

  ret = *status & MCP_RX0IF; *status &= ~MCP_RX0IF;

  if ( ret==0 ) { ret = *status & MCP_RX1IF; *status &= ~MCP_RX1IF; }

  return ret;
}

/*********************************************************************************************************
** Function name:           checkClearTxStatus
** Descriptions:            Return specified buffer of first found tx CANINTF status and clears it from parameter.
**                          Note that this does not affect to chip CANINTF at all. You can use this
**                          with one single readRxTxStatus call.
*********************************************************************************************************/
byte MCP_CAN::checkClearTxStatus(byte *status, byte iTxBuf)
{
  byte ret;

  if ( iTxBuf<MCP_N_TXBUFFERS ) { // Clear specific buffer flag
    ret = *status & txIfFlag(iTxBuf); *status &= ~txIfFlag(iTxBuf);
  } else {
    ret=0;
    for (byte i = 0; i < MCP_N_TXBUFFERS-nReservedTx; i++) {
      ret = *status & txIfFlag(i);
      if ( ret!=0 ) {
        *status &= ~txIfFlag(i);
        return ret;
      }
    };
  }

  return ret;
}

/*********************************************************************************************************
** Function name:           clearBufferTransmitIfFlags
** Descriptions:            Clear transmit interrupt flags for specific buffer or for all unreserved buffers.
**                          If interrupt will be used, it is important to clear all flags, when there is no
**                          more data to be sent. Otherwise IRQ will newer change state.
*********************************************************************************************************/
void MCP_CAN::clearBufferTransmitIfFlags(byte flags)
{
  flags &= MCP_TX_INT;
  if ( flags==0 ) return;
  mcp2515_modifyRegister(MCP_CANINTF, flags, 0);
}

/*********************************************************************************************************
** Function name:           checkReceive
** Descriptions:            check if got something
*********************************************************************************************************/
byte MCP_CAN::checkReceive(void)
{
    byte res;
    res = mcp2515_readStatus();                                         // RXnIF in Bit 1 and 0
    return ((res & MCP_STAT_RXIF_MASK)?CAN_MSGAVAIL:CAN_NOMSG);
}

/*********************************************************************************************************
** Function name:           checkError
** Descriptions:            if something error
*********************************************************************************************************/
byte MCP_CAN::checkError(void)
{
    byte eflg = mcp2515_readRegister(MCP_EFLG);
    return ((eflg & MCP_EFLG_ERRORMASK) ? CAN_CTRLERROR : CAN_OK);
}

/*********************************************************************************************************
** Function name:           getCanId
** Descriptions:            when receive something, you can get the can id!!
*********************************************************************************************************/
unsigned long MCP_CAN::getCanId(void)
{
    return can_id;
}

/*********************************************************************************************************
** Function name:           isRemoteRequest
** Descriptions:            when receive something, you can check if it was a request
*********************************************************************************************************/
byte MCP_CAN::isRemoteRequest(void)
{
    return rtr;
}

/*********************************************************************************************************
** Function name:           isExtendedFrame
** Descriptions:            did we just receive standard 11bit frame or extended 29bit? 0 = std, 1 = ext
*********************************************************************************************************/
byte MCP_CAN::isExtendedFrame(void)
{
    return ext_flg;
}

/*********************************************************************************************************
  END FILE
*********************************************************************************************************/
