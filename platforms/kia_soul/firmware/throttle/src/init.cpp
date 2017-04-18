#include "throttle_module.h"
#include "init.h"

void init_pins( kia_soul_throttle_module_s *module )
{
    // set up pin modes
    pinMode( module->pins.dac_cs, OUTPUT );
    pinMode( module->pins.signal_accel_pos_sensor_high, INPUT );
    pinMode( module->pins.signal_accel_pos_sensor_low, INPUT );
    pinMode( module->pins.spoof_signal_high, INPUT );
    pinMode( module->pins.spoof_signal_low, INPUT );
    pinMode( module->pins.spoof_enable, OUTPUT );

    // initialize the DAC board
    digitalWrite( module->pins.dac_cs, HIGH );     // Deselect DAC CS

    // Initialize relay board
    digitalWrite( module->pins.spoof_enable, LOW );
}
