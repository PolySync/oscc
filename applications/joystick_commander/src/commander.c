#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_joystick.h>
#include <SDL2/SDL_gamecontroller.h>
#include <sys/time.h>
#include <linux/can.h>

#include "oscc.h"
#include "vehicles/vehicles.h"

#include "joystick.h"
#include "pid.h"

#define PID_WINDUP_GUARD ( 1500 )
#define PID_PROPORTIONAL_GAIN ( 0.3 )
#define PID_INTEGRAL_GAIN ( 1.3 )
#define PID_DERIVATIVE_GAIN ( 0.03 )
#define STEERING_ANGLE_MIN ( -360.0 )
#define STEERING_ANGLE_MAX ( 360.0 )
#define JOYSTICK_AXIS_THROTTLE (SDL_CONTROLLER_AXIS_TRIGGERRIGHT)
#define JOYSTICK_AXIS_BRAKE (SDL_CONTROLLER_AXIS_TRIGGERLEFT)
#define JOYSTICK_AXIS_STEER (SDL_CONTROLLER_AXIS_LEFTX)
#define JOYSTICK_BUTTON_ENABLE_CONTROLS (SDL_CONTROLLER_BUTTON_START)
#define JOYSTICK_BUTTON_DISABLE_CONTROLS (SDL_CONTROLLER_BUTTON_BACK)
#define BRAKES_ENABLED_MIN (0.05)
#define JOYSTICK_DELAY_INTERVAL (50000)
#define COMMANDER_ENABLED ( 1 )
#define COMMANDER_DISABLED ( 0 )

#define USEC_TO_SEC(usec) ( (usec) / 1000000.0 )

static int commander_enabled = COMMANDER_DISABLED;
static bool control_enabled = false;

static pid_s steering_pid;
static double prev_angle;
static double curr_angle;

static int get_normalized_position( unsigned long axis_index, double * const normalized_position );
static int check_trigger_positions( );
static int commander_disable_controls( );
static int commander_enable_controls( );
static int get_button( unsigned long button, unsigned int* const state );
static int command_brakes( );
static int command_throttle( );
static int command_steering( );
static void brake_callback(oscc_brake_report_s *report);
static void throttle_callback(oscc_throttle_report_s *report);
static void steering_callback(oscc_steering_report_s *report);
static void fault_callback(oscc_fault_report_s *report);
static void obd_callback(struct can_frame *frame);
static bool check_for_brake_faults( );
static bool check_for_steering_faults( );
static bool check_for_throttle_faults( );

int commander_init( int channel )
{
    int return_code = OSCC_ERROR;

    if ( commander_enabled == COMMANDER_DISABLED )
    {
        commander_enabled = COMMANDER_ENABLED;

        return_code = oscc_open( channel );

        if ( return_code != OSCC_ERROR )
        {
            oscc_subscribe_to_obd_messages(obd_callback);
            oscc_subscribe_to_brake_reports(brake_callback);
            oscc_subscribe_to_steering_reports(steering_callback);
            oscc_subscribe_to_throttle_reports(throttle_callback);
            oscc_subscribe_to_fault_reports(fault_callback);

            return_code = joystick_init( );

            printf( "waiting for joystick controls to zero\n" );

            while ( return_code != OSCC_ERROR )
            {
                return_code = check_trigger_positions( );

                if ( return_code == OSCC_WARNING )
                {
                    (void) usleep( JOYSTICK_DELAY_INTERVAL );
                }
                else if ( return_code == OSCC_ERROR )
                {
                    printf( "Failed to wait for joystick to zero the control values\n" );
                }
                else
                {
                    break;
                }
            }
            pid_zeroize(&steering_pid, PID_WINDUP_GUARD);
            steering_pid.proportional_gain = PID_PROPORTIONAL_GAIN;
            steering_pid.integral_gain = PID_INTEGRAL_GAIN;
            steering_pid.derivative_gain = PID_DERIVATIVE_GAIN;
        }
    }
    return ( return_code );
}

void commander_close( int channel )
{
    if ( commander_enabled == COMMANDER_ENABLED )
    {
        commander_disable_controls( );

        oscc_disable( );

        oscc_close( channel );

        joystick_close( );

        commander_enabled = COMMANDER_DISABLED;
    }
}

int check_for_controller_update( )
{
    unsigned int button_pressed = 0;

    int return_code = joystick_update( );

    if ( return_code == OSCC_OK )
    {
        return_code = get_button( JOYSTICK_BUTTON_DISABLE_CONTROLS,
                                  &button_pressed );

        if ( return_code == OSCC_OK )
        {
            if ( button_pressed != 0 )
            {
                printf( "Disabling Controls\n" );
                return_code = commander_disable_controls( );
            }
            else
            {
                button_pressed = 0;
                return_code = get_button( JOYSTICK_BUTTON_ENABLE_CONTROLS,
                                          &button_pressed );
                if ( return_code == OSCC_OK )
                {
                    if ( button_pressed != 0 )
                    {
                        return_code = commander_enable_controls( );
                    }
                    else
                    {
                        if ( control_enabled )
                        {
                            return_code = command_brakes( );

                            if ( return_code == OSCC_OK )
                            {
                                return_code = command_throttle( );
                            }

                            if ( return_code == OSCC_OK )
                            {
                                return_code = command_steering( );
                            }
                        }
                    }
                }
            }
        }
    }
    return return_code;
}

static int get_normalized_position( unsigned long axis_index, double * const normalized_position )
{
    int return_code = OSCC_ERROR;

    int axis_position = 0;

    double low = 0.0, high = 1.0;

    return_code = joystick_get_axis( axis_index, &axis_position );

    if ( return_code == OSCC_OK )
    {
        if ( axis_index == JOYSTICK_AXIS_STEER )
        {
            low = -1.0;
        }

    ( *normalized_position ) = m_constrain(
            ((double) axis_position) / INT16_MAX,
            low,
            high);
    }

    return ( return_code );

}

static int check_trigger_positions( )
{
    int return_code = OSCC_ERROR;

    return_code = joystick_update( );

    if ( return_code == OSCC_OK )
    {
        double normalized_brake_position = 0;
        return_code = get_normalized_position( JOYSTICK_AXIS_BRAKE, &normalized_brake_position );

        if ( return_code == OSCC_OK )
        {
            double normalized_throttle_position = 0;
            return_code = get_normalized_position( JOYSTICK_AXIS_THROTTLE, &normalized_throttle_position );

            if ( return_code == OSCC_OK )
            {
                if ( ( normalized_throttle_position > 0.0 ) ||
                     ( normalized_brake_position > 0.0 ) )
                {
                    return_code = OSCC_WARNING;
                }
            }
        }
    }
    return return_code;
}

static int commander_disable_controls( )
{
    int return_code = OSCC_ERROR;

    printf( "Disable controls\n" );

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = oscc_disable();

        if ( return_code == OSCC_OK )
        {
            control_enabled = false;
        }
    }
    return return_code;
}

static int commander_enable_controls( )
{
    int return_code = OSCC_ERROR;

    printf( "Enable controls\n" );

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        return_code = oscc_enable();

        if ( return_code == OSCC_OK )
        {
            control_enabled = true;
        }
    }
    return ( return_code );
}

static int get_button( unsigned long button, unsigned int* const state )
{
    int return_code = OSCC_ERROR;

    if ( state != NULL )
    {
        unsigned int button_state;

        return_code = joystick_get_button( button, &button_state );

        if ( ( return_code == OSCC_OK ) &&
             ( button_state == JOYSTICK_BUTTON_STATE_PRESSED ) )
        {
            ( *state ) = 1;
        }
        else
        {
            ( *state ) = 0;
        }
    }
    return ( return_code );
}

static int command_brakes( )
{
    int return_code = OSCC_ERROR;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        double normalized_position = 0;

        return_code = get_normalized_position( JOYSTICK_AXIS_BRAKE, &normalized_position );

        return_code = oscc_publish_brake_position( normalized_position );
    }
    return ( return_code );
}

static int command_throttle( )
{
    int return_code = OSCC_ERROR;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        double normalized_throttle_position = 0;

        return_code = get_normalized_position( JOYSTICK_AXIS_THROTTLE, &normalized_throttle_position );

        if ( return_code == OSCC_OK && normalized_throttle_position > 0.0 )
        {
            double normalized_brake_position = 0;

            return_code = get_normalized_position( JOYSTICK_AXIS_BRAKE, &normalized_brake_position );

            if ( normalized_brake_position >= BRAKES_ENABLED_MIN )
            {
                normalized_throttle_position = 0.0;
            }
        }

        return_code = oscc_publish_throttle_position( normalized_throttle_position );
    }

    return ( return_code );
}

static int command_steering( )
{
    int return_code = OSCC_ERROR;

    static struct timeval time_now, last_loop_time;

    if ( commander_enabled == COMMANDER_ENABLED )
    {
        gettimeofday(&time_now, NULL);

        double time_between_loops_in_sec =
            USEC_TO_SEC((time_now.tv_usec - last_loop_time.tv_usec));

        double normalized_position = 0;

        return_code = get_normalized_position( JOYSTICK_AXIS_STEER, &normalized_position );

        double commanded_angle = normalized_position * STEERING_ANGLE_MAX;

        if ( time_between_loops_in_sec > 0.0 )
        {
            double steering_wheel_angle_rate =
                ( curr_angle - prev_angle ) / time_between_loops_in_sec;

            double steering_wheel_angle_rate_target =
                ( commanded_angle - curr_angle ) / time_between_loops_in_sec;

            prev_angle = curr_angle;

            pid_update(
                    &steering_pid,
                    steering_wheel_angle_rate_target,
                    steering_wheel_angle_rate,
                    time_between_loops_in_sec );

            double torque = steering_pid.control;

            torque = m_constrain(
                torque,
                MINIMUM_TORQUE_COMMAND,
                MAXIMUM_TORQUE_COMMAND
            );

            //normalize torque
            torque /= (double) MAXIMUM_TORQUE_COMMAND;

            return_code = oscc_publish_steering_torque( torque );
        }

        gettimeofday(&last_loop_time, NULL);
    }
    return ( return_code );
}

static void throttle_callback(oscc_throttle_report_s *report)
{
    if ( report->operator_override )
    {
        oscc_disable();
    }
}

static void steering_callback(oscc_steering_report_s *report)
{
    if ( report->operator_override )
    {
        oscc_disable();
    }
}

static void brake_callback(oscc_brake_report_s * report)
{
    if ( report->operator_override )
    {
        oscc_disable();
    }
}

static void fault_callback(oscc_fault_report_s *report)
{
    oscc_disable();
}

static void obd_callback(struct can_frame *frame)
{
    if ( frame->can_id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID )
    {
        kia_soul_obd_steering_wheel_angle_data_s * steering_data = (kia_soul_obd_steering_wheel_angle_data_s*) frame->data;

        curr_angle = steering_data->steering_wheel_angle * 0.1;
    }
}
