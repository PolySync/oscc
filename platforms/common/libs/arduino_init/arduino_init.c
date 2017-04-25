#include <Arduino.h>
#include "arduino_init.h"


void init_arduino( void )
{
    init();

    #if defined(USBCON)
    USBDevice.attach();
    #endif
}
