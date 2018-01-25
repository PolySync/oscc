/*********************************************************************
This is a library for our Monochrome OLEDs based on SSD1325 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

These sendBuffers use SPI to communicate, 4 or 5 pins are required to
interface

Adafruit invests time and resources providing this open source code,
please support Adafruit and open-source hardware by purchasing
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution
*********************************************************************/


#if ARDUINO >= 100
 #include "Arduino.h"
#else
 #include "WProgram.h"
#endif

#ifdef __arm__
#define _BV(b) (1<<(b))
#endif

#include "gfx.h"

#ifndef gfx_swap
#define gfx_swap(a, b) { uint8_t t = a; a = b; b = t; }
#endif

#define BLACK 0
#define WHITE 1

#define SSD1325_LCDWIDTH                  128
#define SSD1325_LCDHEIGHT                 64

#define SSD1325_SETCOLADDR 0x15
#define SSD1325_SETROWADDR 0x75
#define SSD1325_SETCONTRAST 0x81
#define SSD1325_SETCURRENT 0x84

#define SSD1325_SETREMAP 0xA0
#define SSD1325_SETSTARTLINE 0xA1
#define SSD1325_SETOFFSET 0xA2
#define SSD1325_NORMALDISPLAY 0xA4
#define SSD1325_DISPLAYALLON 0xA5
#define SSD1325_DISPLAYALLOFF 0xA6
#define SSD1325_INVERTDISPLAY 0xA7
#define SSD1325_SETMULTIPLEX 0xA8
#define SSD1325_MASTERCONFIG 0xAD
#define SSD1325_DISPLAYOFF 0xAE
#define SSD1325_DISPLAYON 0xAF

#define SSD1325_SETPRECHARGECOMPENABLE 0xB0
#define SSD1325_SETPHASELEN 0xB1
#define SSD1325_SETROWPERIOD 0xB2
#define SSD1325_SETCLOCK 0xB3
#define SSD1325_SETPRECHARGECOMP 0xB4
#define SSD1325_SETGRAYTABLE 0xB8
#define SSD1325_SETPRECHARGEVOLTAGE 0xBC
#define SSD1325_SETVCOMLEVEL 0xBE
#define SSD1325_SETVSL 0xBF

#define SSD1325_GFXACCEL 0x23
#define SSD1325_DRAWRECT 0x24
#define SSD1325_COPY 0x25

class SSD1325 : public GFX {
    public:
        SSD1325(int8_t CS) : GFX(128,64), cs(CS) {}

        void begin(void);
        void eraseBuffer(void);
        void sendBuffer(void);
        void drawPixel(int16_t x, int16_t y, uint16_t color);
        bool readButton(void);
        void enableRedLed(void);
        void enableYellowLed(void);
        void enableGreenLed(void);
        void disableLeds(void);

    private:
        int8_t cs;
        uint8_t shift_register_data;

        void setDC(uint8_t value);
        void setRes(uint8_t value);
        void startSendCommand(void);
        void stopSendCommand(void);
        void startSendData(void);
        void stopSendData(void);
};
