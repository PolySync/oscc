#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <net/if.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include "oscc.h"
#include "internal/oscc.h"


static int can_socket = -1;


oscc_result_t oscc_open( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;


    char can_string_buffer[16];

    snprintf( can_string_buffer, 16, "can%u", channel );

    printf( "Opening CAN channel: %s\n", can_string_buffer );

    result = oscc_init_can( can_string_buffer );


    return result;
}

oscc_result_t oscc_close( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;


    if( can_socket != -1 )
    {
        int result = close( can_socket );

        if ( result > 0 )
        {
            result = OSCC_OK;
        }
    }


    return result;
}

oscc_result_t oscc_enable( void )
{
    oscc_result_t result = OSCC_ERROR;


    result = oscc_enable_brakes( );

    if ( result == OSCC_OK )
    {
        result = oscc_enable_throttle( );

        if (result == OSCC_OK )
        {
            result = oscc_enable_steering( );
        }
    }


    return result;
}

oscc_result_t oscc_disable( void )
{
    oscc_result_t result = OSCC_ERROR;


    result = oscc_disable_brakes( );

    if ( result == OSCC_OK )
    {
        result = oscc_disable_throttle( );

        if ( result == OSCC_OK )
        {
            result = oscc_disable_steering( );
        }
    }


    return result;
}

oscc_result_t oscc_publish_brake_position( double brake_position )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_brake_command_s brake_cmd =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

#if defined(KIA_SOUL)
    const double clamped_position = ( double ) CONSTRAIN (
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


    const uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    const uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    brake_cmd.spoof_value_low = spoof_value_low;
    brake_cmd.spoof_value_high = spoof_value_high;
#endif

    result = oscc_can_write(
        OSCC_BRAKE_COMMAND_CAN_ID,
        (void *) &brake_cmd,
        sizeof(brake_cmd) );


    return result;
}

oscc_result_t oscc_publish_throttle_position( double throttle_position )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_throttle_command_s throttle_cmd =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };


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


    const uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    const uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    throttle_cmd.spoof_value_low = spoof_value_low;
    throttle_cmd.spoof_value_high = spoof_value_high;

    result = oscc_can_write(
        OSCC_THROTTLE_COMMAND_CAN_ID,
        (void *) &throttle_cmd,
        sizeof(throttle_cmd) );


    return result;
}

oscc_result_t oscc_publish_steering_torque( double torque )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_steering_command_s steering_cmd =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };


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


    const uint16_t spoof_value_low = STEPS_PER_VOLT * spoof_voltage_low;
    const uint16_t spoof_value_high = STEPS_PER_VOLT * spoof_voltage_high;

    steering_cmd.spoof_value_low = spoof_value_low;
    steering_cmd.spoof_value_high = spoof_value_high;

    result = oscc_can_write(
        OSCC_STEERING_COMMAND_CAN_ID,
        (void *) &steering_cmd,
        sizeof(steering_cmd) );


    return result;
}

oscc_result_t oscc_subscribe_to_brake_reports( void (*callback)(oscc_brake_report_s *report) )
{
    oscc_result_t result = OSCC_ERROR;


    if ( callback != NULL )
    {
        brake_report_callback = callback;
        result = OSCC_OK;
    }


    return result;
}

oscc_result_t oscc_subscribe_to_throttle_reports( void (*callback)(oscc_throttle_report_s *report) )
{
    oscc_result_t result = OSCC_ERROR;


    if ( callback != NULL )
    {
        throttle_report_callback = callback;
        result = OSCC_OK;
    }


    return result;
}

oscc_result_t oscc_subscribe_to_steering_reports( void (*callback)(oscc_steering_report_s *report))
{
    oscc_result_t result = OSCC_ERROR;


    if ( callback != NULL )
    {
        steering_report_callback = callback;
        result = OSCC_OK;
    }


    return result;
}

oscc_result_t oscc_subscribe_to_fault_reports( void (*callback)(oscc_fault_report_s *report))
{
    oscc_result_t result = OSCC_ERROR;


    if ( callback != NULL )
    {
        fault_report_callback = callback;
        result = OSCC_OK;
    }


    return result;
}

oscc_result_t oscc_subscribe_to_obd_messages( void (*callback)(struct can_frame *frame))
{
    oscc_result_t result = OSCC_ERROR;


    if ( callback != NULL )
    {
        obd_frame_callback = callback;
        result = OSCC_OK;
    }


    return result;
}




/* Internal */
oscc_result_t oscc_enable_brakes( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_brake_enable_s brake_enable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_BRAKE_ENABLE_CAN_ID,
        (void *) &brake_enable,
        sizeof(brake_enable) );


    return result;
}

oscc_result_t oscc_enable_throttle( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_throttle_enable_s throttle_enable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_THROTTLE_ENABLE_CAN_ID,
        (void *) &throttle_enable,
        sizeof(throttle_enable) );


    return result;
}

oscc_result_t oscc_enable_steering( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_steering_enable_s steering_enable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_STEERING_ENABLE_CAN_ID,
        (void *) &steering_enable,
        sizeof(steering_enable) );


    return result;
}

oscc_result_t oscc_disable_brakes( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_brake_disable_s brake_disable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_BRAKE_DISABLE_CAN_ID,
        (void *) &brake_disable,
        sizeof(brake_disable) );


    return result;
}

oscc_result_t oscc_disable_throttle( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_throttle_disable_s throttle_disable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_THROTTLE_DISABLE_CAN_ID,
        (void *) &throttle_disable,
        sizeof(throttle_disable) );


    return result;
}

oscc_result_t oscc_disable_steering( void )
{
    oscc_result_t result = OSCC_ERROR;


    oscc_steering_disable_s steering_disable =
    {
        .magic[0] = ( uint8_t ) OSCC_MAGIC_BYTE_0,
        .magic[1] = ( uint8_t ) OSCC_MAGIC_BYTE_1
    };

    result = oscc_can_write(
        OSCC_STEERING_DISABLE_CAN_ID,
        (void *) &steering_disable,
        sizeof(steering_disable) );


    return result;
}

void oscc_update_status( )
{
    struct can_frame rx_frame;
    memset( &rx_frame, 0, sizeof(rx_frame) );

    if ( can_socket != -1 )
    {
        int ret = read( can_socket, &rx_frame, CAN_MTU );

        while ( ret > 0 )
        {
            if ( (rx_frame.data[0] == OSCC_MAGIC_BYTE_0)
                && (rx_frame.data[1] == OSCC_MAGIC_BYTE_1) )
            {
                if ( rx_frame.can_id == OSCC_STEERING_REPORT_CAN_ID )
                {
                    oscc_steering_report_s *steering_report =
                        ( oscc_steering_report_s* ) rx_frame.data;

                    if ( steering_report_callback != NULL )
                    {
                        steering_report_callback( steering_report );
                    }
                }
                else if ( rx_frame.can_id == OSCC_THROTTLE_REPORT_CAN_ID )
                {
                    oscc_throttle_report_s *throttle_report =
                        ( oscc_throttle_report_s* ) rx_frame.data;

                    if ( throttle_report_callback != NULL )
                    {
                        throttle_report_callback( throttle_report );
                    }
                }
                else if ( rx_frame.can_id == OSCC_BRAKE_REPORT_CAN_ID )
                {
                    oscc_brake_report_s *brake_report =
                        ( oscc_brake_report_s* ) rx_frame.data;

                    if ( brake_report_callback != NULL )
                    {
                        brake_report_callback( brake_report );
                    }
                }
                else if ( rx_frame.can_id == OSCC_FAULT_REPORT_CAN_ID )
                {
                    oscc_fault_report_s *fault_report =
                        ( oscc_fault_report_s* ) rx_frame.data;

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

            ret = read( can_socket, &rx_frame, CAN_MTU );
        }
    }
}

oscc_result_t oscc_can_write( long id, void *msg, unsigned int dlc )
{
    oscc_result_t result = OSCC_ERROR;


    if ( can_socket != -1 )
    {
        struct can_frame tx_frame;

        memset( &tx_frame, 0, sizeof(tx_frame) );
        tx_frame.can_id = id;
        tx_frame.can_dlc = dlc;
        memcpy( tx_frame.data, msg, dlc );

        int ret = write( can_socket, &tx_frame, sizeof(tx_frame) );

        if ( ret > 0 )
        {
            result = OSCC_OK;
        }
        else
        {
            printf( "Could not write to socket: %s\n", strerror(errno) );
        }
    }


    return result;
}

oscc_result_t oscc_async_enable( int socket )
{
    oscc_result_t result = OSCC_ERROR;


    int ret = fcntl( socket, F_SETOWN, getpid( ) );

    if ( ret < 0 )
    {
        printf( "Setting owner process of socket failed: %s\n", strerror(errno) );
    }
    else
    {
        result = OSCC_OK;
    }


    if ( result == OSCC_OK )
    {
        ret = fcntl( socket, F_SETFL, FASYNC | O_NONBLOCK );

        if ( ret < 0 )
        {
            printf( "Setting nonblocking asynchronous socket I/O failed: %s\n", strerror(errno) );

            result = OSCC_ERROR;
        }
    }


    return result;
}

oscc_result_t oscc_init_can( const char *can_channel )
{
    int result = OSCC_ERROR;
    int ret = -1;


    struct sigaction sig;

    memset( &sig, 0, sizeof(sig) );
    sigemptyset( &sig.sa_mask );
    sig.sa_flags = SA_RESTART;
    sig.sa_handler = oscc_update_status;
    sigaction( SIGIO, &sig, NULL );

    int sock = socket( PF_CAN, SOCK_RAW, CAN_RAW );

    if ( sock < 0 )
    {
        printf( "Opening CAN socket failed: %s\n", strerror(errno) );
    }
    else
    {
        result = OSCC_OK;
    }


    struct ifreq ifr;
    memset( &ifr, 0, sizeof(ifr) );

    if ( result == OSCC_OK )
    {
        strncpy( ifr.ifr_name, can_channel, IFNAMSIZ );

        ret = ioctl( sock, SIOCGIFINDEX, &ifr );

        if ( ret < 0 )
        {
            printf( "Finding CAN index failed: %s\n", strerror(errno) );

            result = OSCC_ERROR;
        }
    }


    if ( result == OSCC_OK )
    {
        struct sockaddr_can can_address;

        memset( &can_address, 0, sizeof(can_address) );
        can_address.can_family = AF_CAN;
        can_address.can_ifindex = ifr.ifr_ifindex;

        ret = bind(
            sock,
            (struct sockaddr *) &can_address,
            sizeof(can_address) );

        if ( ret < 0 )
        {
            printf( "Socket binding failed: %s\n", strerror(errno) );

            result = OSCC_ERROR;
        }
    }


    if ( result == OSCC_OK )
    {
        ret = oscc_async_enable( sock );

        if ( ret != OSCC_OK )
        {
            printf( "Enabling asynchronous socket I/O failed\n" );

            result = OSCC_ERROR;
        }
    }


    if ( result == OSCC_OK )
    {
        /* all prior checks will pass even if a valid interface has not been
           set up - attempt to write an empty CAN frame to the interface to see
           if it is valid */
        struct can_frame tx_frame;

        memset( &tx_frame, 0, sizeof(tx_frame) );
        tx_frame.can_id = 0;
        tx_frame.can_dlc = 8;

        int bytes_written = write( sock, &tx_frame, sizeof(tx_frame) );

        if ( bytes_written < 0 )
        {
            printf( "Failed to write test frame to %s: %s\n", can_channel, strerror(errno) );

            result = OSCC_ERROR;
        }
        else
        {
            can_socket = sock;
        }
    }


    return result;
}
