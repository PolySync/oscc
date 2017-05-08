#include <stdint.h>
#include <sys/timeb.h>

#include "Arduino.h"


unsigned long g_mock_arduino_millis_return;
uint8_t g_mock_arduino_digital_write_pin;
uint8_t g_mock_arduino_digital_write_val;
int g_mock_arduino_analog_read_return;


unsigned long millis(void)
{
    return g_mock_arduino_millis_return;
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    g_mock_arduino_digital_write_pin = pin;
    g_mock_arduino_digital_write_val = val;
}

int analogRead(uint8_t pin)
{
    return g_mock_arduino_analog_read_return;
}

void delay(unsigned long ms)
{
}

void _Serial::print(const char str[])
{
    printf("%s", str);
}

void _Serial::println(const char str[])
{
    printf("%s\n", str);
}

void _Serial::println(float f)
{
    printf("%f\n", f);
}

void _Serial::println(uint16_t d)
{
    printf("%d\n", d);
}

void _Serial::println(int16_t d)
{
    printf("%d\n", d);
}

_Serial Serial;
