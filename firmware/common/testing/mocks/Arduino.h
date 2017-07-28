#ifndef _OSCC_TEST_MOCK_ARDUINO_H_
#define _OSCC_TEST_MOCK_ARDUINO_H_

#include <stdio.h>
#include <stdint.h>


#define A0 0
#define A1 1
#define A2 2
#define A3 3
#define LOW 0
#define HIGH 1
#define INPUT 0
#define OUTPUT 1

#define constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

unsigned long millis(void);

unsigned long micros(void);

void pinMode(uint8_t, uint8_t);

void digitalWrite(uint8_t pin, uint8_t val);

int analogRead(uint8_t pin);

void analogWrite(uint8_t pin, int val);

void delay(unsigned long ms);

void sei();

void cli();

class _Serial
{
    public:
        void begin(unsigned long);
        void print(const char[]);
        void println(const char[]);
        void println(float f);
        void println(uint16_t d);
        void println(int16_t d);
};

extern _Serial Serial;

#endif
