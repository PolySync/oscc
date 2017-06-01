/**
 * @file master_cylinder.cpp
 *
 */


#include <Arduino.h>
#include "debug.h"
#include "signal_smoothing.h"

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

    float unfiltered_pressure_1 = raw_adc_to_pressure( raw_adc_sensor_1 );
    float unfiltered_pressure_2 = raw_adc_to_pressure( raw_adc_sensor_2 );

    const float filter_alpha = MASTER_CYLINDER_PRESSURE_SENSOR_EXPONENTIAL_FILTER_ALPHA;
    static float filtered_pressure_1 = 0.0;
    static float filtered_pressure_2 = 0.0;

    filtered_pressure_1 = exponential_moving_average(
        filter_alpha,
        unfiltered_pressure_1,
        filtered_pressure_1);

    filtered_pressure_2 = exponential_moving_average(
        filter_alpha,
        unfiltered_pressure_2,
        filtered_pressure_2);

    pressure->sensor_1_pressure = filtered_pressure_1;
    pressure->sensor_2_pressure = filtered_pressure_2;
}
