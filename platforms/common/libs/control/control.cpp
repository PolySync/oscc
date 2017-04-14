#include <stdint.h>

#include "DAC_MCP49xx.h"
#include "control.h"
#include "debug.h"


static int32_t get_analog_sample_average(
        int32_t num_samples,
        uint8_t pin );

static void write_sample_averages_to_dac(
        int16_t num_samples,
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        DAC_MCP49xx *dac );


void enable_control(
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        uint8_t relay_pin,
        control_state_s *state,
        DAC_MCP49xx *dac )
{
    // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac( num_samples, signal_pin_1, signal_pin_2, dac);

    // Enable the signal interrupt relays
    digitalWrite( relay_pin, HIGH );

    state->enabled = true;

    DEBUG_PRINTLN( "Control enabled" );
}


void disable_control(
        uint8_t signal_pin_1,
        uint8_t signal_pin_2,
        uint8_t relay_pin,
        control_state_s *state,
        DAC_MCP49xx *dac )
{
    // Sample the current values, smooth them, and write measured torque values to DAC to avoid a
    // signal discontinuity when the SCM takes over
    static uint16_t num_samples = 20;
    write_sample_averages_to_dac( num_samples, signal_pin_1, signal_pin_2, dac);

    // Disable the signal interrupt relays
    digitalWrite( relay_pin, LOW );

    state->enabled =false;

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
        DAC_MCP49xx *dac )
{
    int32_t averages[ 2 ] = { 0, 0 };

    averages[0] = get_analog_sample_average( num_samples, signal_pin_1);
    averages[1] = get_analog_sample_average( num_samples, signal_pin_2);

    // Write measured values to DAC to avoid a signal discontinuity when the SCM takes over
    dac->outputA( averages[0] );
    dac->outputB( averages[1] );
}
