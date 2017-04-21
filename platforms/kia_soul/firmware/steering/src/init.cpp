#include "init.h"
#include "globals.h"


void init_pins( )
{
    pinMode( PIN_DAC_CS, OUTPUT );
    pinMode( PIN_TORQUE_POS_SENSOR_HIGH, INPUT );
    pinMode( PIN_TORQUE_POS_SENSOR_LOW, INPUT );
    pinMode( PIN_TORQUE_POS_SPOOF_HIGH, INPUT );
    pinMode( PIN_TORQUE_POS_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    digitalWrite( PIN_DAC_CS, HIGH );

    digitalWrite( PIN_SPOOF_ENABLE, LOW );
}
