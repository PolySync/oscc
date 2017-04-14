#include <Arduino.h>
#include <stdint.h>
#include "time.h"


void get_update_time_delta_ms(
        const uint32_t time_in,
        const uint32_t last_update_ms,
        uint32_t * const delta_out )
{
    // check for overflow
    if( last_update_ms < time_in )
    {
        // time remainder, prior to the overflow
        (*delta_out) = (UINT32_MAX - time_in);

        // add time since zero
        (*delta_out) += last_update_ms;
    }
    else
    {
        // normal delta
        (*delta_out) = (last_update_ms - time_in);
    }
}


uint32_t timer_delta_ms( uint32_t last_time, uint32_t* current_time )
{
    uint32_t delta = 0;
    uint32_t local_time = millis( );

    if ( local_time < last_time )
    {
        // Timer overflow
        delta = ( UINT32_MAX - last_time ) + local_time;
    }
    else
    {
        delta = local_time - last_time;
    }

    if ( current_time != NULL )
    {
        *current_time = local_time;
    }

    return ( delta );
}


uint32_t timer_delta_us( uint32_t last_time, uint32_t* current_time )
{
    uint32_t delta = 0;
    uint32_t local_time = micros( );

    if ( local_time < last_time )
    {
        // Timer overflow
        delta = ( UINT32_MAX - last_time ) + local_time;
    }
    else
    {
        delta = local_time - last_time;
    }

    if ( current_time != NULL )
    {
        *current_time = local_time;
    }

    return ( delta );
}
