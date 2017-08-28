#include <stdint.h>
#include <sys/timeb.h>
#include <stdio.h>

#include "Arduino.h"


unsigned long g_mock_arduino_millis_return;
unsigned long g_mock_arduino_micros_return;
uint8_t g_mock_arduino_digital_write_pins[100];
uint8_t g_mock_arduino_digital_write_val[100];
int g_mock_arduino_digital_write_count;
int g_mock_arduino_analog_read_return[100];
uint8_t g_mock_arduino_analog_write_pins[100];
int g_mock_arduino_analog_write_val[100];
int g_mock_arduino_analog_write_count;



unsigned long millis(void)
{
    return g_mock_arduino_millis_return;
}

unsigned long micros(void)
{
    return g_mock_arduino_micros_return;
}

void pinMode(uint8_t, uint8_t)
{

}

void digitalWrite(uint8_t pin, uint8_t val)
{
    g_mock_arduino_digital_write_pins[g_mock_arduino_digital_write_count] = pin;
    g_mock_arduino_digital_write_val[g_mock_arduino_digital_write_count] = val;

    // need to keep track of successive calls to digitalWrite to be able to check
    // all of their respective values
    ++g_mock_arduino_digital_write_count;
}

int analogRead(uint8_t pin)
{
    return g_mock_arduino_analog_read_return[pin];
}

void analogWrite(uint8_t pin, int val)
{
    g_mock_arduino_analog_write_pins[g_mock_arduino_analog_write_count] = pin;
    g_mock_arduino_analog_write_val[g_mock_arduino_analog_write_count] = val;

    // need to keep track of successive calls to analogWrite to be able to check
    // all of their respective values
    ++g_mock_arduino_analog_write_count;
}

void delay(unsigned long ms)
{
}

void cli(void)
{
}

void sei(void)
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
