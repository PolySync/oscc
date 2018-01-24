/**
 * @file oscc_check.cpp
 *
 */


#include <Arduino.h>

#include "oscc_check.h"
#include "vehicles.h"


bool check_voltage_grounded( uint16_t high, uint16_t low ) {

    static unsigned long elapsed_detection_time = 0;
    unsigned long current_time = millis();

    bool ret = false;
    if( (high == 0) || (low == 0) )
    {
        if ( elapsed_detection_time == 0 )
        {
            elapsed_detection_time = millis();
        }

        ret = ( current_time - elapsed_detection_time ) > FAULT_HYSTERESIS;
    }
    else
    {
        elapsed_detection_time = 0;
    }

    return ret;
}
