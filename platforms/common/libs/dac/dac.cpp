#include <Arduino.h>

#include "dac.h"


static int32_t get_analog_sample_average(
    const int32_t num_samples,
    const uint8_t pin );


void write_sample_averages_to_dac(
        DAC_MCP49xx & dac,
        const int16_t num_samples,
        const uint8_t signal_pin_1,
        const uint8_t signal_pin_2 )
{
    int32_t averages[ 2 ] = { 0, 0 };

    averages[0] = get_analog_sample_average( num_samples, signal_pin_1)
        << BIT_SHIFT_10BIT_TO_12BIT;

    averages[1] = get_analog_sample_average( num_samples, signal_pin_2)
        << BIT_SHIFT_10BIT_TO_12BIT;

    // Write measured values to DAC to avoid a signal discontinuity when the SCM takes over
    dac.outputA( averages[0] );
    dac.outputB( averages[1] );
}


static int32_t get_analog_sample_average(
    const int32_t num_samples,
    const uint8_t pin )
{
    int32_t sum = 0;
    int32_t i = 0;

    for ( i = 0; i < num_samples; ++i )
    {
        sum += analogRead( pin );
    }

    return (sum / num_samples);
}
