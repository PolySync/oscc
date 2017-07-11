/**
 * @file master_cylinder.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"

#include "globals.h"
#include "helper.h"
#include "master_cylinder.h"


void master_cylinder_init( void )
{
    pinMode( PIN_MASTER_CYLINDER_SOLENOID, OUTPUT );

    master_cylinder_open( );
}


void master_cylinder_open( void )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, SOLENOID_PWM_OFF );

    DEBUG_PRINTLN( "Master Cylinder Open" );
}


void master_cylinder_close( void )
{
    analogWrite( PIN_MASTER_CYLINDER_SOLENOID, SOLENOID_PWM_ON );

    DEBUG_PRINTLN( "Master Cylinder Close" );
}


void master_cylinder_read_pressure( master_cylinder_pressure_s * pressure )
{
    int raw_adc_sensor_1 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_1 );
    int raw_adc_sensor_2 = analogRead( PIN_MASTER_CYLINDER_PRESSURE_SENSOR_2 );

    pressure->sensor_1_pressure = raw_adc_to_pressure( raw_adc_sensor_1 );
    pressure->sensor_2_pressure = raw_adc_to_pressure( raw_adc_sensor_2 );
}
