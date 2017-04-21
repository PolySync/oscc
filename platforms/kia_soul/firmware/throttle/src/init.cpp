#include "globals.h"
#include "init.h"

void init_pins( )
{
    pinMode( PIN_DAC_CS, OUTPUT );
    pinMode( PIN_ACCEL_POS_SENSOR_HIGH, INPUT );
    pinMode( PIN_ACCEL_POS_SENSOR_LOW, INPUT );
    pinMode( PIN_ACCEL_POS_SPOOF_HIGH, INPUT );
    pinMode( PIN_ACCEL_POS_SPOOF_LOW, INPUT );
    pinMode( PIN_SPOOF_ENABLE, OUTPUT );

    digitalWrite( PIN_DAC_CS, HIGH ); // Deselect DAC CS

    digitalWrite( PIN_SPOOF_ENABLE, LOW );
}
