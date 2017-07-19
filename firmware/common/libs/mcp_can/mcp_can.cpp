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

#define spi_readwrite   SPI.transfer
#define spi_read()      spi_readwrite(0x00)
#define SPI_BEGIN()     SPI.beginTransaction(SPISettings(10000000, MSBFIRST, SPI_MODE0))
#define SPI_END()       SPI.endTransaction()

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
    for(i=0; i<n && i<CAN_MAX_CHAR_IN_MESSAGE; i++)
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

    for(i=0; i<n; i++)
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

    if(i == newmode)
    {
        return MCP2515_OK;
    }

    return MCP2515_FAIL;
}

/*********************************************************************************************************
** Function name:           mcp2515_configRate
** Descriptions:            set boadrate
*********************************************************************************************************/
byte MCP_CAN::mcp2515_configRate(const byte canSpeed)
{
    byte set, cfg1, cfg2, cfg3;
    set = 1;
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

    if(set) {
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
    for(i = 0; i < 14; i++)                         // in-buffer loop
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
byte MCP_CAN::mcp2515_init(const byte canSpeed)
{

    byte res;

    mcp2515_reset();

    res = mcp2515_setCANCTRL_Mode(MODE_CONFIG);
    if(res > 0)
    {
#if DEBUG_EN
        Serial.print("Enter setting mode fall\r\n");
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
    if(mcp2515_configRate(canSpeed))
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

    if(res == MCP2515_OK) {

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
        if(res)
        {
#if DEBUG_EN
            Serial.print("Enter Normal Mode Fall!!\r\n");
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
** Function name:           mcp2515_write_id
** Descriptions:            write can id
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_id(const byte mcp_addr, const byte ext, const unsigned long id)
{
    uint16_t canid;
    byte tbufdata[4];

    canid = (uint16_t)(id & 0x0FFFF);

    if(ext == 1)
    {
        tbufdata[MCP_EID0] = (byte) (canid & 0xFF);
        tbufdata[MCP_EID8] = (byte) (canid >> 8);
        canid = (uint16_t)(id >> 16);
        tbufdata[MCP_SIDL] = (byte) (canid & 0x03);
        tbufdata[MCP_SIDL] += (byte) ((canid & 0x1C) << 3);
        tbufdata[MCP_SIDL] |= MCP_TXB_EXIDE_M;
        tbufdata[MCP_SIDH] = (byte) (canid >> 5);
    }
    else
    {
        tbufdata[MCP_SIDH] = (byte) (canid >> 3);
        tbufdata[MCP_SIDL] = (byte) ((canid & 0x07) << 5);
        tbufdata[MCP_EID0] = 0;
        tbufdata[MCP_EID8] = 0;
    }
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

    *id = (tbufdata[MCP_SIDH]<<3) + (tbufdata[MCP_SIDL]>>5);

    if((tbufdata[MCP_SIDL] & MCP_TXB_EXIDE_M) ==  MCP_TXB_EXIDE_M)
    {
        // extended id
        *id = (*id<<2) + (tbufdata[MCP_SIDL] & 0x03);
        *id = (*id<<8) + tbufdata[MCP_EID8];
        *id = (*id<<8) + tbufdata[MCP_EID0];
        *ext = 1;
    }
}

/*********************************************************************************************************
** Function name:           mcp2515_write_canMsg
** Descriptions:            write msg
*********************************************************************************************************/
void MCP_CAN::mcp2515_write_canMsg(const byte buffer_sidh_addr, int rtrBit)
{
    byte mcp_addr;
    mcp_addr = buffer_sidh_addr;
    mcp2515_setRegisterS(mcp_addr+5, dta, dta_len);                  // write data bytes
    // Serial.print("RTR: ");
    // Serial.println(rtrBit);
    if(rtrBit == 1)                                                   // if RTR set bit in byte
    {
        dta_len |= MCP_RTR_MASK;
    }
    mcp2515_setRegister((mcp_addr+4), dta_len);                        // write the RTR and DLC
    mcp2515_write_id(mcp_addr, ext_flg, can_id);                     // write CAN id

}

/*********************************************************************************************************
** Function name:           mcp2515_read_canMsg
** Descriptions:            read message
*********************************************************************************************************/
void MCP_CAN::mcp2515_read_canMsg(const byte buffer_sidh_addr)        // read can msg
{
    byte mcp_addr, ctrl;

    mcp_addr = buffer_sidh_addr;
    mcp2515_read_id(mcp_addr, &ext_flg,&can_id);
    ctrl = mcp2515_readRegister(mcp_addr-1);
    dta_len = mcp2515_readRegister(mcp_addr+4);

    rtr = (ctrl & 0x08) ? 1 : 0;

    dta_len &= MCP_DLC_MASK;
    mcp2515_readRegisterS(mcp_addr+5, &(dta[0]), dta_len);
}

/*********************************************************************************************************
** Function name:           mcp2515_start_transmit
** Descriptions:            start transmit
*********************************************************************************************************/
void MCP_CAN::mcp2515_start_transmit(const byte mcp_addr)              // start transmit
{
    mcp2515_modifyRegister(mcp_addr-1 , MCP_TXB_TXREQ_M, MCP_TXB_TXREQ_M);
}

/*********************************************************************************************************
** Function name:           mcp2515_getNextFreeTXBuf
** Descriptions:            get Next free txbuf
*********************************************************************************************************/
byte MCP_CAN::mcp2515_getNextFreeTXBuf(byte *txbuf_n)                 // get Next free txbuf
{
    byte res, i, ctrlval;
    byte ctrlregs[MCP_N_TXBUFFERS] = { MCP_TXB0CTRL, MCP_TXB1CTRL, MCP_TXB2CTRL };

    res = MCP_ALLTXBUSY;
    *txbuf_n = 0x00;

    // check all 3 TX-Buffers
    for(i=0; i<MCP_N_TXBUFFERS; i++)
    {
        ctrlval = mcp2515_readRegister(ctrlregs[i]);
        if((ctrlval & MCP_TXB_TXREQ_M) == 0) {
            *txbuf_n = ctrlregs[i]+1;                                   // return SIDH-address of Buffer
            res = MCP2515_OK;
            return res;                                                 // ! function exit
        }
    }
    return res;
}

/*********************************************************************************************************
** Function name:           set CS
** Descriptions:            init CS pin and set UNSELECTED
*********************************************************************************************************/
MCP_CAN::MCP_CAN(byte _CS)
{
    SPICS = _CS;
}

/*********************************************************************************************************
** Function name:           init
** Descriptions:            init can and set speed
*********************************************************************************************************/
byte MCP_CAN::begin(byte speedset)
{
    pinMode(SPICS, OUTPUT);
    MCP2515_UNSELECT();
    SPI.begin();
    byte res = mcp2515_init(speedset);
    return ((res == MCP2515_OK) ? CAN_OK : CAN_FAILINIT);
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
    if(res > 0){
#if DEBUG_EN
        Serial.print("Enter setting mode fall\r\n");
#else
        delay(10);
#endif
        return res;
    }

    if(num == 0){
        mcp2515_write_id(MCP_RXM0SIDH, ext, ulData);

    }
    else if(num == 1){
        mcp2515_write_id(MCP_RXM1SIDH, ext, ulData);
    }
    else res =  MCP2515_FAIL;

    res = mcp2515_setCANCTRL_Mode(MODE_NORMAL);
    if(res > 0){
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
    if(res > 0)
    {
#if DEBUG_EN
        Serial.print("Enter setting mode fall\r\n");
#else
        delay(10);
#endif
        return res;
    }

    switch(num)
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
    if(res > 0)
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
** Function name:           setMsg
** Descriptions:            set can message, such as dlc, id, dta[] and so on
*********************************************************************************************************/
byte MCP_CAN::setMsg(unsigned long id, byte ext, byte len, byte rtr, byte *pData)
{
    ext_flg     = ext;
    can_id      = id;
    dta_len     = min(len, MAX_CHAR_IN_MESSAGE);
    rtr         = rtr;
    for(int i = 0; i<dta_len; i++)
    {
        dta[i] = *(pData+i);
    }
    return MCP2515_OK;
}


/*********************************************************************************************************
** Function name:           setMsg
** Descriptions:            set can message, such as dlc, id, dta[] and so on
*********************************************************************************************************/
byte MCP_CAN::setMsg(unsigned long id, byte ext, byte len, byte *pData)
{
    return setMsg(id, ext, len, 0, pData);
}

/*********************************************************************************************************
** Function name:           clearMsg
** Descriptions:            set all message to zero
*********************************************************************************************************/
byte MCP_CAN::clearMsg()
{
    can_id      = 0;
    dta_len     = 0;
    ext_flg     = 0;
    rtr         = 0;
    filhit      = 0;

    for(int i = 0; i<dta_len; i++)
    {
        dta[i] = 0x00;
    }

    return MCP2515_OK;
}

/*********************************************************************************************************
** Function name:           sendMsg
** Descriptions:            send message
*********************************************************************************************************/
byte MCP_CAN::sendMsg(int rtrBit)
{
    byte res, res1, txbuf_n;
    uint16_t uiTimeOut = 0;

    do {
        res = mcp2515_getNextFreeTXBuf(&txbuf_n);                       // info = addr.
        uiTimeOut++;
    } while (res == MCP_ALLTXBUSY && (uiTimeOut < TIMEOUTVALUE));

    if(uiTimeOut == TIMEOUTVALUE)
    {
        return CAN_GETTXBFTIMEOUT;                                      // get tx buff time out
    }

    uiTimeOut = 0;
    mcp2515_write_canMsg(txbuf_n, rtrBit);
    mcp2515_start_transmit(txbuf_n);

    do {
        uiTimeOut++;
        res1= mcp2515_readRegister(txbuf_n-1 /* the ctrl reg is located at txbuf_n-1 */);  // read send buff ctrl reg
        res1 = res1 & 0x08;
    }while(res1 && (uiTimeOut < TIMEOUTVALUE));

    if(uiTimeOut == TIMEOUTVALUE)                                       // send msg timeout
    {
        return CAN_SENDMSGTIMEOUT;
    }
    return CAN_OK;

}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            send buf
*********************************************************************************************************/
byte MCP_CAN::sendMsgBuf(unsigned long id, byte ext, byte rtr, byte len, byte *buf)
{
    setMsg(id, ext, len, rtr, buf);
    // Serial.print("RTR00: ");
    // Serial.println(rtr, HEX);
    return sendMsg(rtr);
}

/*********************************************************************************************************
** Function name:           sendMsgBuf
** Descriptions:            send buf
*********************************************************************************************************/
byte MCP_CAN::sendMsgBuf(unsigned long id, byte ext, byte len, byte *buf)
{
    setMsg(id, ext, len, buf);
    return sendMsg(0);
}


/*********************************************************************************************************
** Function name:           readMsg
** Descriptions:            read message
*********************************************************************************************************/
byte MCP_CAN::readMsg()
{
    byte stat, res;

    stat = mcp2515_readStatus();

    if(stat & MCP_STAT_RX0IF)                                        // Msg in Buffer 0
    {
        mcp2515_read_canMsg(MCP_RXBUF_0);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX0IF, 0);
        res = CAN_OK;
    }
    else if(stat & MCP_STAT_RX1IF)                                   // Msg in Buffer 1
    {
        mcp2515_read_canMsg(MCP_RXBUF_1);
        mcp2515_modifyRegister(MCP_CANINTF, MCP_RX1IF, 0);
        res = CAN_OK;
    }
    else
    {
        res = CAN_NOMSG;
    }
    return res;
}

/*********************************************************************************************************
** Function name:           readMsgBuf
** Descriptions:            read message buf
*********************************************************************************************************/
byte MCP_CAN::readMsgBuf(byte *len, byte buf[])
{
    byte  rc;

    rc = readMsg();

    if(rc == CAN_OK) {
        *len = dta_len;
        for(int i = 0; i<dta_len; i++)
        {
            buf[i] = dta[i];
        }
    } else {
        *len = 0;
    }
    return rc;
}

/*********************************************************************************************************
** Function name:           readMsgBufID
** Descriptions:            read message buf and can bus source ID
*********************************************************************************************************/
byte MCP_CAN::readMsgBufID(unsigned long *ID, byte *len, byte buf[])
{
    byte rc;
    rc = readMsg();

    if(rc == CAN_OK) {
        *len = dta_len;
        *ID  = can_id;
        for(int i = 0; i<dta_len && i < MAX_CHAR_IN_MESSAGE; i++)
        {
            buf[i] = dta[i];
        }
    } else {
        *len = 0;
    }
    return rc;
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
** Descriptions:            when receive something you can get the can id!!
*********************************************************************************************************/
unsigned long MCP_CAN::getCanId(void)
{
    return can_id;
}

/*********************************************************************************************************
** Function name:           isRemoteRequest
** Descriptions:            when receive something you can check if it was a request
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