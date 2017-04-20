#include <Arduino.h>
#include "debug.h"
#include "time.h"
#include "PID.h"

#include "brake_control.h"
#include "brake_module.h"
#include "master_cylinder.h"
#include "helper.h"

void brake_lights_off( kia_soul_brake_module_s *brake_module )
{
    digitalWrite( brake_module->pins.brake_light, LOW );
}


void brake_lights_on( kia_soul_brake_module_s *brake_module )
{
    digitalWrite( brake_module->pins.brake_light, HIGH );
}


void brake_command_actuator_solenoids( kia_soul_brake_module_s *brake_module, uint16_t duty_cycle )
{
    analogWrite( brake_module->pins.slafl, duty_cycle );
    analogWrite( brake_module->pins.slafr, duty_cycle );
}


void brake_command_release_solenoids( kia_soul_brake_module_s *brake_module, uint16_t duty_cycle )
{
    analogWrite( brake_module->pins.slrfl, duty_cycle );
    analogWrite( brake_module->pins.slrfr, duty_cycle );
}


void brake_enable( kia_soul_brake_module_s *brake_module )
{
    if ( brake_module->control_state.enabled == false )
    {
        master_cylinder_close( brake_module );
        brake_command_release_solenoids( brake_module, 0 );
        brake_module->control_state.enabled = true;
    }
}


void brake_disable( kia_soul_brake_module_s *brake_module )
{
    if ( brake_module->control_state.enabled == true )
    {
        brake_command_actuator_solenoids( brake_module, 0 );

        brake_command_release_solenoids( brake_module, 255 );

        brake_lights_off( brake_module );
        delay( 15 );

        master_cylinder_open( brake_module );

        brake_command_release_solenoids( brake_module, 0 );

        brake_module->control_state.enabled = false;
    }
}


void brake_check_driver_override( kia_soul_brake_module_s *brake_module )
{
    static const float filter_alpha = 0.05;

    static float filtered_input_1 = 0.0;
    static float filtered_input_2 = 0.0;

    float sensor_1 = ( float )( analogRead( brake_module->pins.pmc1 ) );
    float sensor_2 = ( float )( analogRead( brake_module->pins.pmc2 ) );

    sensor_1 = raw_adc_to_pressure( ( uint16_t )sensor_1, brake_module );
    sensor_2 = raw_adc_to_pressure( ( uint16_t )sensor_2, brake_module );

    filtered_input_1 = ( filter_alpha * sensor_1 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_1 );

    filtered_input_2 = ( filter_alpha * sensor_2 ) +
        ( ( 1.0 - filter_alpha ) * filtered_input_2 );

    if ( ( filtered_input_1 > brake_module->params.driver_override_pedal_threshold ) ||
         ( filtered_input_2 > brake_module->params.driver_override_pedal_threshold ) )
    {
        DEBUG_PRINTLN( "driver override" );
        brake_module->control_state.driver_override = 1;
        brake_module->control_state.enable_request = false;
    }
    else
    {
        brake_module->control_state.driver_override = 0;
    }
}


void brake_update_pressure( kia_soul_brake_module_s *brake_module )
{
    uint16_t raw_left_pressure = analogRead( brake_module->pins.pfl );
    uint16_t raw_right_pressure = analogRead( brake_module->pins.pfr );

    float pressure_left = raw_adc_to_pressure( raw_left_pressure, brake_module );
    float pressure_right = raw_adc_to_pressure( raw_right_pressure, brake_module );

    brake_module->state.current_pressure = ( pressure_left + pressure_right ) / 2;
}


void brake_init( kia_soul_brake_module_s *brake_module )
{
    // initialize solenoid pins to off
    digitalWrite( brake_module->pins.slafl, LOW );
    digitalWrite( brake_module->pins.slafr, LOW );
    digitalWrite( brake_module->pins.slrfl, LOW );
    digitalWrite( brake_module->pins.slrfr, LOW );

    // set pinmode to OUTPUT
    pinMode( brake_module->pins.slafl, OUTPUT );
    pinMode( brake_module->pins.slafr, OUTPUT );
    pinMode( brake_module->pins.slrfl, OUTPUT );
    pinMode( brake_module->pins.slrfr, OUTPUT );

    brake_lights_off( brake_module );
    pinMode( brake_module->pins.brake_light, OUTPUT );
}


void brake_update( kia_soul_brake_module_s *brake_module )
{
    static float pressure_target = 0.0;
    static float pressure = 0.0;

    static uint32_t control_loop_time = GET_TIMESTAMP_US();

    float loop_delta_t = (float)get_time_delta( control_loop_time, control_loop_time );

    loop_delta_t /= 1000.0;
    loop_delta_t /= 1000.0;

    brake_update_pressure( brake_module );

    // ********************************************************************
    //
    // WARNING
    //
    // The ranges selected to do brake control are carefully tested to
    // ensure that the pressure actuated is not outside of the range of
    // what the brake module can handle. By changing any of this code you
    // risk attempting to actuate a pressure outside of the brake modules
    // valid range. Actuating a pressure outside of the modules valid
    // range will, at best, cause it to go into an unrecoverable fault
    // state. This is characterized by the accumulator "continuously
    // pumping" without accumulating any actual pressure, or being
    // "over pressured." Clearing this fault state requires expert
    // knowledge of the braking module.
    //
    // It is NOT recommended to modify any of the existing control ranges,
    // or gains, without expert knowledge.
    //
    // ************************************************************************

    static interpolate_range_s pressure_ranges =
        { 0.0, UINT16_MAX, 12.0, 878.3 };

    pressure = brake_module->state.current_pressure;

    pressure_target = interpolate( brake_module->state.pedal_command, &pressure_ranges );

    PID pid_params;

    // Initialize PID params
    pid_zeroize( &pid_params, brake_module->params.pid_windup_guard );

    pid_params.proportional_gain = brake_module->params.pid_proportional_gain;
    pid_params.integral_gain     = brake_module->params.pid_integral_gain;
    pid_params.derivative_gain   = brake_module->params.pid_derivative_gain;

    int16_t ret = pid_update( &pid_params,
                              pressure_target,
                              pressure,
                              loop_delta_t );

    // Requested pressure
    DEBUG_PRINT(pressure_target);

    // Pressure at wheels (PFR and PFL)
    DEBUG_PRINT(",");
    DEBUG_PRINT(pressure);

    // PID output
    DEBUG_PRINT(",");
    DEBUG_PRINT( pid_params.control );

    if ( ret == PID_SUCCESS )
    {
        float pid_output = pid_params.control;

        if ( pid_output < -10.0 )
        {
            // pressure is too high
            static interpolate_range_s slr_ranges =
                { 0.0, 60.0, brake_module->params.slr_duty_cycle_min, brake_module->params.slr_duty_cycle_max };

            uint16_t slr_duty_cycle = 0;

            brake_command_actuator_solenoids( brake_module, 0 );

            pid_output = -pid_output;
            slr_duty_cycle = (uint16_t)interpolate( pid_output, &slr_ranges );

            if ( slr_duty_cycle > ( uint16_t )brake_module->params.slr_duty_cycle_max )
            {
                slr_duty_cycle = ( uint16_t )brake_module->params.slr_duty_cycle_max;
            }

            brake_command_release_solenoids( brake_module, slr_duty_cycle );

            DEBUG_PRINT(",0,");
            DEBUG_PRINT(slr_duty_cycle);

            if ( pressure_target < 20.0 )
            {
                brake_lights_off( brake_module );
            }

        }
        else if ( pid_output > 10.0 )
        {
            // pressure is too low
            static interpolate_range_s sla_ranges =
                { 10.0, 110.0, brake_module->params.sla_duty_cycle_min, brake_module->params.sla_duty_cycle_max };

            uint16_t sla_duty_cycle = 0;

            brake_lights_on( brake_module );

            brake_command_release_solenoids( brake_module, 0 );

            sla_duty_cycle = (uint16_t)interpolate( pid_output, &sla_ranges );

            if ( sla_duty_cycle > ( uint16_t )brake_module->params.sla_duty_cycle_max )
            {
                sla_duty_cycle = ( uint16_t )brake_module->params.sla_duty_cycle_max;
            }

            brake_command_actuator_solenoids( brake_module, sla_duty_cycle );

            DEBUG_PRINT(",");
            DEBUG_PRINT(sla_duty_cycle);
            DEBUG_PRINT(",0");
        }
        else    // -10.0 < pid_output < 10.0
        {
            if ( brake_module->state.pedal_command < 100 )
            {
                brake_lights_off( brake_module );
            }
        }
    }

    DEBUG_PRINTLN("");
}
