#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <errno.h>
#include <signal.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "vehicles.h"
#include "dtc.h"
#include "oscc.h"

#define CONSTRAIN(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

static int can_socket;

static oscc_brake_command_s brake_cmd;
static oscc_throttle_command_s throttle_cmd;
static oscc_steering_command_s steering_cmd;

static void (*steering_report_callback)(oscc_steering_report_s *report);
static void (*brake_report_callback)(oscc_brake_report_s *report);
static void (*throttle_report_callback)(oscc_throttle_report_s *report);
static void (*fault_report_callback)(oscc_fault_report_s *report);
static void (*obd_frame_callback)(struct can_frame *frame);

static oscc_result_t oscc_init_can( const char *can_channel );
static oscc_result_t oscc_can_write( long id, void *msg, unsigned int dlc );
static oscc_result_t oscc_async_enable( int socket );
static oscc_result_t oscc_enable_brakes( void );
static oscc_result_t oscc_enable_throttle( void );
static oscc_result_t oscc_enable_steering( void );
static oscc_result_t oscc_disable_brakes( void );
static oscc_result_t oscc_disable_throttle( void );
static oscc_result_t oscc_disable_steering( void );
static void oscc_update_status( );
static void oscc_init_commands( void );

oscc_result_t oscc_open( unsigned int channel )
{
    oscc_result_t ret = OSCC_ERROR;

    char buffer[16];

    snprintf( buffer, 16, "can%1d", channel );

    printf( "Opening CAN channel: %s\n", buffer );

    ret = oscc_init_can( buffer );

    oscc_init_commands( );

    return ret;
}

oscc_result_t oscc_close( unsigned int channel )
{
    oscc_result_t ret = OSCC_ERROR;

    int result = close( can_socket );

    if ( result > 0 )
    {
        ret = OSCC_OK;
    }

    return ret;
}

oscc_result_t oscc_enable( void )
{
    oscc_result_t ret = oscc_enable_brakes( );

    if (ret == OSCC_OK )
    {
        ret = oscc_enable_throttle( );

        if (ret == OSCC_OK )
        {
            ret = oscc_enable_steering( );
        }
    }

    return ret;
}

oscc_result_t oscc_disable( void )
{
    oscc_result_t ret = oscc_disable_brakes( );

    if ( ret == OSCC_OK )
    {
        ret = oscc_disable_throttle( );

        if ( ret == OSCC_OK )
        {
            ret = oscc_disable_steering( );
        }
    }

    return ret;
}

oscc_result_t oscc_publish_brake_position( double brake_position )
{
    oscc_result_t ret = OSCC_ERROR;

#if defined(KIA_SOUL_PETROL)
    const double clamped_position = (double) CONSTRAIN (
            brake_position * MAXIMUM_BRAKE_COMMAND,
            MINIMUM_BRAKE_COMMAND,
            MAXIMUM_BRAKE_COMMAND );

    brake_cmd.pedal_command = ( uint16_t ) BRAKE_POSITION_TO_PEDAL( clamped_position );

#elif defined(KIA_SOUL_EV)
    const double clamped_position = CONSTRAIN(
        brake_position,
        MINIMUM_BRAKE_COMMAND,
        MAXIMUM_BRAKE_COMMAND);


    double spoof_voltage_low = BRAKE_POSITION_TO_VOLTS_LOW( clamped_position );

    spoof_voltage_low = CONSTRAIN(
        spoof_voltage_low,
        BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN,
        BRAKE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX);


    double spoof_voltage_high = BRAKE_POSITION_TO_VOLTS_HIGH( clamped_position );

    spoof_voltage_high = CONSTRAIN(
        spoof_voltage_high,
        BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,
        BRAKE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX);


    uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    brake_cmd.spoof_value_low = spoof_value_low;
    brake_cmd.spoof_value_high = spoof_value_high;
#endif

    ret = oscc_can_write(
        OSCC_BRAKE_COMMAND_CAN_ID,
        (void *) &brake_cmd,
        sizeof( brake_cmd ) );

    return ret;
}

oscc_result_t oscc_publish_throttle_position( double throttle_position )
{
    oscc_result_t ret = OSCC_ERROR;

    const double clamped_position = CONSTRAIN(
        throttle_position,
        MINIMUM_THROTTLE_COMMAND,
        MAXIMUM_THROTTLE_COMMAND);


    double spoof_voltage_low = THROTTLE_POSITION_TO_VOLTS_LOW( clamped_position );

    spoof_voltage_low = CONSTRAIN(
        spoof_voltage_low,
        THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MIN,
        THROTTLE_SPOOF_LOW_SIGNAL_VOLTAGE_MAX);


    double spoof_voltage_high = THROTTLE_POSITION_TO_VOLTS_HIGH( clamped_position );

    spoof_voltage_high = CONSTRAIN(
        spoof_voltage_high,
        THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,
        THROTTLE_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX);


    uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    throttle_cmd.spoof_value_low = spoof_value_low;
    throttle_cmd.spoof_value_high = spoof_value_high;

    ret = oscc_can_write(
        OSCC_THROTTLE_COMMAND_CAN_ID,
        (void *)&throttle_cmd,
        sizeof(throttle_cmd));

    return ret;
}

oscc_result_t oscc_publish_steering_torque( double torque )
{
    oscc_result_t ret = OSCC_ERROR;

    const double clamped_torque = CONSTRAIN(
        torque * MAXIMUM_TORQUE_COMMAND,
        MINIMUM_TORQUE_COMMAND,
        MAXIMUM_TORQUE_COMMAND);


    double spoof_voltage_low = STEERING_TORQUE_TO_VOLTS_LOW( clamped_torque );

    spoof_voltage_low = CONSTRAIN(
        spoof_voltage_low,
        STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MIN,
        STEERING_SPOOF_LOW_SIGNAL_VOLTAGE_MAX);


    double spoof_voltage_high = STEERING_TORQUE_TO_VOLTS_HIGH( clamped_torque );

    spoof_voltage_high = CONSTRAIN(
        spoof_voltage_high,
        STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MIN,
        STEERING_SPOOF_HIGH_SIGNAL_VOLTAGE_MAX);


    uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    steering_cmd.spoof_value_low = spoof_value_low;
    steering_cmd.spoof_value_high = spoof_value_high;

    ret = oscc_can_write(
        OSCC_STEERING_COMMAND_CAN_ID,
        (void *)&steering_cmd,
        sizeof(steering_cmd));

    return ret;
}

oscc_result_t oscc_subscribe_to_brake_reports( void (*callback)(oscc_brake_report_s *report) )
{
    oscc_result_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        brake_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_result_t oscc_subscribe_to_throttle_reports( void (*callback)(oscc_throttle_report_s *report) )
{
    oscc_result_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        throttle_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_result_t oscc_subscribe_to_steering_reports( void (*callback)(oscc_steering_report_s *report))
{
    oscc_result_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        steering_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_result_t oscc_subscribe_to_fault_reports( void (*callback)(oscc_fault_report_s *report))
{
    oscc_result_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        fault_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_result_t oscc_subscribe_to_obd_messages( void (*callback)(struct can_frame *frame))
{
    oscc_result_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        obd_frame_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

static oscc_result_t oscc_enable_brakes( void )
{
    oscc_result_t ret = OSCC_ERROR;

    brake_cmd.enable = 1;

    ret = oscc_publish_brake_position( 0 );

    return ret;
}

static oscc_result_t oscc_enable_throttle( void )
{
    oscc_result_t ret = OSCC_ERROR;

    throttle_cmd.enable = 1;

    ret = oscc_publish_throttle_position( 0 );

    return ret;
}

static oscc_result_t oscc_enable_steering( void )
{
    oscc_result_t ret = OSCC_ERROR;

    steering_cmd.enable = 1;

    ret = oscc_publish_steering_torque( 0 );

    return ret;
}

static oscc_result_t oscc_disable_brakes( void )
{
    oscc_result_t ret = OSCC_ERROR;

    brake_cmd.enable = 0;

    ret = oscc_publish_brake_position( 0 );

    return ret;
}

static oscc_result_t oscc_disable_throttle( void )
{
    oscc_result_t ret = OSCC_ERROR;

    throttle_cmd.enable = 0;

    ret = oscc_publish_throttle_position( 0 );

    return ret;
}

static oscc_result_t oscc_disable_steering( void )
{
    oscc_result_t ret = OSCC_ERROR;

    steering_cmd.enable = 0;

    ret = oscc_publish_steering_torque( 0 );

    return ret;
}

static void oscc_update_status( )
{
    struct can_frame rx_frame;

    int result = read( can_socket, &rx_frame, CAN_MTU );

    while ( result > 0 )
    {
        if ( (rx_frame.data[0] == OSCC_MAGIC_BYTE_0)
            && (rx_frame.data[1] = OSCC_MAGIC_BYTE_1) )
        {
            if ( rx_frame.can_id == OSCC_STEERING_REPORT_CAN_ID )
            {
                oscc_steering_report_s *steering_report =
                    (oscc_steering_report_s *)rx_frame.data;

                if ( steering_report_callback != NULL )
                {
                    steering_report_callback( steering_report );
                }
            }
            else if ( rx_frame.can_id == OSCC_THROTTLE_REPORT_CAN_ID )
            {
                oscc_throttle_report_s *throttle_report =
                    ( oscc_throttle_report_s *)rx_frame.data;

                if ( throttle_report_callback != NULL )
                {
                    throttle_report_callback( throttle_report );
                }
            }
            else if ( rx_frame.can_id == OSCC_BRAKE_REPORT_CAN_ID )
            {
                oscc_brake_report_s *brake_report =
                    ( oscc_brake_report_s *)rx_frame.data;

                if ( brake_report_callback != NULL )
                {
                    brake_report_callback( brake_report );
                }
            }
            else if ( rx_frame.can_id == OSCC_FAULT_REPORT_CAN_ID )
            {
                oscc_fault_report_s *fault_report =
                    ( oscc_fault_report_s *)rx_frame.data;

                if ( fault_report_callback != NULL )
                {
                    fault_report_callback( fault_report );
                }
            }
        }
        else
        {
            if ( obd_frame_callback != NULL )
            {
                obd_frame_callback( &rx_frame );
            }
        }

        result = read( can_socket, &rx_frame, CAN_MTU );
    }
}

static oscc_result_t oscc_can_write( long id, void *msg, unsigned int dlc )
{
    oscc_result_t ret = OSCC_ERROR;

    struct can_frame tx_frame;

    tx_frame.can_id = id;
    tx_frame.can_dlc = dlc;
    memcpy( tx_frame.data, msg, dlc);

    int result = write( can_socket, &tx_frame, sizeof(tx_frame ));

    if ( result > 0 )
    {
        ret = OSCC_OK;
    }

    return ret;
}

static oscc_result_t oscc_async_enable( int socket )
{
    oscc_result_t ret = OSCC_ERROR;

   ret = fcntl( socket, F_SETOWN, getpid( ) );

   if ( ret < 0 )
   {
       printf( "set own failed\n" );
   }

   ret = fcntl( socket, F_SETFL, FASYNC | O_NONBLOCK );

   if ( ret < 0 )
   {
       printf( "set async failed\n" );
   }

    return ret;
}

static oscc_result_t oscc_init_can( const char *can_channel )
{
    int ret = OSCC_OK;

    struct sigaction sig;
    sig.sa_handler = oscc_update_status;
    sigaction( SIGIO, &sig, NULL );

    int s = socket( PF_CAN, SOCK_RAW, CAN_RAW );

    if ( s < 0 )
    {
        printf( "opening can socket failed\n" );

        ret = OSCC_ERROR;
    }

    int status;

    struct ifreq ifr;

    if ( ret != OSCC_ERROR )
    {
        strncpy( ifr.ifr_name, can_channel, IFNAMSIZ );

        status = ioctl( s, SIOCGIFINDEX, &ifr );

        if ( status < 0 )
        {
            printf( "finding can index failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if ( ret != OSCC_ERROR )
    {
        struct sockaddr_can can_address;

        can_address.can_family = AF_CAN;
        can_address.can_ifindex = ifr.ifr_ifindex;

        status = bind( s,
                      (struct sockaddr *)&can_address,
                      sizeof(can_address));

        if ( status < 0 )
        {
            printf( "socket binding failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if ( ret != OSCC_ERROR )
    {
        can_socket = s;

        ret = OSCC_OK;
    }

    if ( ret != OSCC_ERROR )
    {
        status = oscc_async_enable( s );

        if ( status != OSCC_OK )
        {
            printf( "async enable failed\n" );

            ret = OSCC_ERROR;
        }
    }

    return ret;
}

static void oscc_init_commands( void )
{
    brake_cmd.magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0;
    brake_cmd.magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1;

    throttle_cmd.magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0;
    throttle_cmd.magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1;

    steering_cmd.magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0;
    steering_cmd.magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1;
}
