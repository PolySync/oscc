/**
 * @file init.cpp
 *
 */


#include <Arduino.h>

#include "debug.h"
#include "globals.h"
#include "status.h"


void init_status( void )
{
#ifdef DRIVEKIT
    pinMode( PIN_LED_RED, OUTPUT);
    pinMode( PIN_LED_GREEN, OUTPUT);
#endif
    status_ledsOn();

}

void status_ledsOff( void)
{
#ifdef DRIVEKIT
    digitalWrite( PIN_LED_RED, HIGH);
    digitalWrite( PIN_LED_GREEN, HIGH);
#endif

}

void status_ledsOn( void )
{
#ifdef DRIVEKIT
    digitalWrite( PIN_LED_RED, LOW);
    digitalWrite( PIN_LED_GREEN, LOW);
#endif

}

void status_setRedLed(uint8_t state)
{
    if(state)
    {
#ifdef DRIVEKIT
        digitalWrite( PIN_LED_RED, LOW);
#endif
    }
    else
    {
#ifdef DRIVEKIT
        digitalWrite( PIN_LED_RED, HIGH);
#endif
    }
}

void status_setGreenLed(uint8_t state)
{
    if(state)
    {
#ifdef DRIVEKIT
        digitalWrite( PIN_LED_GREEN, LOW);
#endif
    }
    else
    {
#ifdef DRIVEKIT
        digitalWrite( PIN_LED_GREEN, HIGH);
#endif
    }
}
