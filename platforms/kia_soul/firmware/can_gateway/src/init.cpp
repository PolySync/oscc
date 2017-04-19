#include <Arduino.h>

#include "can_gateway_module.h"
#include "init.h"

void init_pins( kia_soul_can_gateway_module_s *can_gateway_module)
{
    pinMode( can_gateway_module->pins.status_led, OUTPUT );
}
