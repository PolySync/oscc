#include <Arduino.h>

class SSD1325 {
public:
 SSD1325(int8_t SID, int8_t SCLK, int8_t DC, int8_t RST, int8_t CS){}

 void begin(void);
 void eraseBuffer(void);
 void sendBuffer(void);

private:
 int8_t sid;
 int8_t sclk;
 int8_t dc;
 int8_t rst;
 int8_t cs;

 void startSendCommand(void);
 void stopSendCommand(void);
 void startSendData(void);
 void stopSendData(void);

 void drawPixel(int16_t x, int16_t y, uint16_t color);
};
