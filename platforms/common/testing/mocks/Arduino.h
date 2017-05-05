#ifndef _OSCC_TEST_MOCK_ARDUINO_H_
#define _OSCC_TEST_MOCK_ARDUINO_H_

#include <stdio.h>
#include <stdint.h>


#define A0 0
#define A1 1
#define LOW 0
#define HIGH 1

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

unsigned long millis(void);

void digitalWrite(uint8_t pin, uint8_t val);

int analogRead(uint8_t pin);

void delay(unsigned long ms);

class _Serial
{
    public:
        void begin(unsigned long);
        void print(const char[]);
        void println(const char[]);
        void println(float f);
};

extern _Serial Serial;

#endif
