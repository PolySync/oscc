#include "debug.h"

#include "throttle_module.h"
#include "throttle_control.h"


static int32_t get_analog_sample_average(
        int32_t num_samples,
        uint8_t pin );

static void write_sample_averages_to_dac(
        int16_t num_samples,
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        DAC_MCP49xx &dac );


void calculate_accelerator_spoof(
    float accelerator_target,
    struct accel_spoof_t* spoof )
{
    // values calculated with min/max calibration curve and tuned for neutral
    // balance.  DAC requires 12-bit values, (4096steps/5V = 819.2 steps/V)
    spoof->low = 819.2 * ( 0.0004 * accelerator_target + 0.366 );
    spoof->high = 819.2 * ( 0.0008 * accelerator_target + 0.732 );

    // range = 300 - ~1800
    spoof->low = constrain( spoof->low, 0, 1800 );
    // range = 600 - ~3500
    spoof->high = constrain( spoof->high, 0, 3500 );
}


void check_accelerator_override(
    kia_soul_throttle_module_s *throttle_module,
    DAC_MCP49xx &dac )
{
    uint32_t accel_pos_normalized =
        (throttle_module->state.accel_pos_sensor_low
        + throttle_module->state.accel_pos_sensor_high)
        / 2;

    if ( accel_pos_normalized > throttle_module->params.accelerator_threshold )
    {
        if( throttle_module->control_state.enabled == true )
        {
            disable_control( throttle_module, dac );

            throttle_module->override_flags.accelerator = 1;
        }
    }
    else
    {
        throttle_module->override_flags.accelerator = 0;
    }
}


void enable_control(
        kia_soul_throttle_module_s *throttle_module,
        DAC_MCP49xx &dac )
{
    // Sample the current values, smooth them, and write measured accel values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac(
        num_samples,
        throttle_module->pins.signal_accel_pos_sensor_high,
        throttle_module->pins.signal_accel_pos_sensor_low,
        dac);

    // Enable the signal interrupt relays
    digitalWrite( throttle_module->pins.spoof_enable, HIGH );

    throttle_module->control_state.enabled = true;

    DEBUG_PRINTLN( "Control enabled" );
}


void disable_control(
        kia_soul_throttle_module_s *throttle_module,
        DAC_MCP49xx &dac )
{
    // Sample the current values, smooth them, and write measured accel values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac(
        num_samples,
        throttle_module->pins.signal_accel_pos_sensor_high,
        throttle_module->pins.signal_accel_pos_sensor_low,
        dac);

    // Disable the signal interrupt relays
    digitalWrite( throttle_module->pins.spoof_enable, LOW );

    throttle_module->control_state.enabled =false;

    DEBUG_PRINTLN( "Control disabled" );
}


static int32_t get_analog_sample_average(
    int32_t num_samples,
    uint8_t pin )
{
    int32_t sum = 0;
    int32_t i = 0;

    for ( i = 0; i < num_samples; ++i )
    {
        sum += analogRead( pin );
    }

    return ( (sum / num_samples) << 2 );
}


static void write_sample_averages_to_dac(
        int16_t num_samples,
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        DAC_MCP49xx &dac )
{
    int32_t averages[ 2 ] = { 0, 0 };

    averages[0] = get_analog_sample_average( num_samples, signal_pin_1);
    averages[1] = get_analog_sample_average( num_samples, signal_pin_2);

    // Write measured values to DAC to avoid a signal discontinuity when the SCM takes over
    dac.outputA( averages[0] );
    dac.outputB( averages[1] );
}
