/**
 * @file timer.cpp
 *
 */


#include "oscc_timer.h"

#include "timer.h"
#include "display.h"


void start_timer( void )
{
    timer1_init( DISPLAY_UPDATE_FREQUENCY_IN_HZ, update_display );
}
