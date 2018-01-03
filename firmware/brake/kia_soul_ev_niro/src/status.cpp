/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "can_protocols/throttle_can_protocol.h"
#include "communications.h"
#include "debug.h"
#include "globals.h"
#include "status.h"
#include "init.h"




void init_status( void )
{
    pinMode( PIN_LED_RED, OUTPUT);
    pinMode( PIN_LED_GREEN, OUTPUT);
    status_ledsOn();

}

void status_ledsOff( void)
{
    digitalWrite( PIN_LED_RED, HIGH);
    digitalWrite( PIN_LED_GREEN, HIGH);

}

void status_ledsOn( void )
{
    digitalWrite( PIN_LED_RED, LOW);
    digitalWrite( PIN_LED_GREEN, LOW);

}

void status_setRedLed(uint8_t state)
{
    if(state)
    {
        digitalWrite( PIN_LED_RED, LOW);
    }
    else
    {
        digitalWrite( PIN_LED_RED, HIGH);
    }
}

void status_setGreenLed(uint8_t state)
{
    if(state)
    {
        digitalWrite( PIN_LED_GREEN, LOW);
    }
    else
    {
        digitalWrite( PIN_LED_GREEN, HIGH);
    }
}
