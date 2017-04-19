#include "steering_module.h"
#include "init.h"

void init_pins( kia_soul_steering_module_s *module )
{
    pinMode( module->pins.dac_cs, OUTPUT );
    pinMode( module->pins.signal_torque_pos_sensor_high, INPUT );
    pinMode( module->pins.signal_torque_pos_sensor_low, INPUT );
    pinMode( module->pins.spoof_signal_high, INPUT );
    pinMode( module->pins.spoof_signal_low, INPUT );
    pinMode( module->pins.spoof_enable, OUTPUT );

    // Initialize the DAC board by setting the DAC Chip Select
    digitalWrite( module->pins.dac_cs, HIGH );

    // Initialize relay board by clearing the Spoof Engage
    digitalWrite( module->pins.spoof_enable, LOW );
}
