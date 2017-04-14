#include <Arduino.h>
#include <stdint.h>
#include "time.h"


uint32_t get_time_delta(
        const uint32_t timestamp_a,
        const uint32_t timestamp_b)
{
    uint32_t delta = 0;

    // check for overflow
    if( timestamp_b < timestamp_a )
    {
        // time remainder, prior to the overflow
        delta = UINT32_MAX - timestamp_a;

        // add time since zero
        delta += timestamp_b;
    }
    else
    {
        // normal delta
        delta = timestamp_b - timestamp_a;
    }

    return delta;
}


bool is_timeout( uint32_t timestamp_a, uint32_t timestamp_b, int timeout )
{
    bool ret = false;

    uint32_t delta = get_time_delta(timestamp_a, timestamp_b);

    if( delta >= timeout )
    {
        ret = true;
    }

    return ret;
}