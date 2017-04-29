/**
 * @file master_cylinder.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"

#include "globals.h"
#include "master_cylinder.h"


void master_cylinder_init( void )
{
    pinMode( PIN_MASTER_CYLINDER_SOLENOID, OUTPUT );

    master_cylinder_open( );
}


void master_cylinder_open( void )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, 0 );

    DEBUG_PRINTLN( "Master Cylinder Open" );
}


void master_cylinder_close( void )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, 255 );

    DEBUG_PRINTLN( "Master Cylinder Close" );
}
