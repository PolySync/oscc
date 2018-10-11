/**
 * @file main.cpp
 * NULL Implementation
 */


#include "arduino_init.h"
#include "Time.h"
#include "debug.h"


int main( void )
{
    uint16_t current_sys_time = now();

    init_arduino( );

    DEBUG_PRINTLN( "Null loop" );
    DEBUG_PRINTLN( now() );

    while( true ) { }
}
