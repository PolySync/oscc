#include <Arduino.h>
#include "debug.h"

#include "master_cylinder.h"
#include "brake_module.h"


void master_cylinder_init( kia_soul_brake_module_s *brake_module )
{
    pinMode( brake_module->pins.smc, OUTPUT );

    master_cylinder_open( brake_module );
}


void master_cylinder_open( kia_soul_brake_module_s *brake_module )
{
    analogWrite( brake_module->pins.smc, 0 );

    DEBUG_PRINTLN( "MC Open" );
}


void master_cylinder_close( kia_soul_brake_module_s *brake_module )
{
    analogWrite( brake_module->pins.smc, 255 );

    DEBUG_PRINTLN( "MC Close" );
}



