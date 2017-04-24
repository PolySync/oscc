#include <Arduino.h>
#include "debug.h"

#include "globals.h"
#include "master_cylinder.h"


void master_cylinder_init( )
{
    pinMode( PIN_MASTER_CYLINDER_SOLENOID, OUTPUT );

    master_cylinder_open( );
}


void master_cylinder_open( )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, 0 );

    DEBUG_PRINTLN( "MC Open" );
}


void master_cylinder_close( )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, 255 );

    DEBUG_PRINTLN( "MC Close" );
}
