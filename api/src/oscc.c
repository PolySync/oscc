#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <signal.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <poll.h>

#include "oscc.h"
#include "internal/oscc.h"


static int global_oscc_can_socket = UNINITIALIZED_SOCKET;
static int global_vehicle_can_socket = UNINITIALIZED_SOCKET;


oscc_result_t oscc_init()
{
    oscc_result_t result = OSCC_ERROR;

    result = oscc_search_can( &auto_init_all_can, true );

    if( result == OSCC_OK )
    {
        result = register_can_signal();
    }

    if ( result == OSCC_OK && global_oscc_can_socket > 0 )
    {
        result = oscc_async_enable( global_oscc_can_socket );
    }
    else
    {
        printf( "Error: Could not find OSCC CAN signal\n" );
        result = OSCC_ERROR;
    }

    if ( result == OSCC_OK && global_vehicle_can_socket > 0 )
    {
        oscc_async_enable( global_vehicle_can_socket );
    }

    return result;
}

oscc_result_t oscc_open( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;

    can_contains_s channel_contents =
        {
            .is_oscc = false,
            .has_vehicle = false
        };

    char can_string_buffer[16];

    snprintf( can_string_buffer, 16, "can%u", channel );

    channel_contents = can_detection( can_string_buffer );

    if( !channel_contents.has_vehicle )
    {
        int vehicle_ret = OSCC_ERROR;

        vehicle_ret = oscc_search_can( &auto_init_vehicle_can, false );

        if( (global_vehicle_can_socket < 0) || (vehicle_ret != OSCC_OK) )
        {
            printf( "Warning: Vehicle CAN was not found.\n" );
        }
    }

    result = init_oscc_can( can_string_buffer );

    if( result == OSCC_OK)
    {
        result = register_can_signal();
    }

    if ( result == OSCC_OK && global_oscc_can_socket >= 0 )
    {
        result = oscc_async_enable( global_oscc_can_socket );
    }
    else
    {
        printf( "Error: Could not find OSCC CAN signal.\n" );
    }

    if ( result == OSCC_OK && global_vehicle_can_socket >= 0 )
    {
        oscc_async_enable( global_vehicle_can_socket );
    }

    return result;
}


oscc_result_t oscc_close( unsigned int channel )
{
    bool closed_channel = false;
    bool close_errored = false;

    if( global_oscc_can_socket >= 0 )
    {
        int result = close( global_oscc_can_socket );

        if ( result == 0 )
        {
            closed_channel = true;
        }
        else
        {
            close_errored = true;
        }
    }

    if( global_vehicle_can_socket >= 0 )
    {
        int result = close( global_vehicle_can_socket );

        if ( result == 0 )
        {
            closed_channel = true;
        }
        else
        {
            close_errored = true;
        }
    }

    if ( closed_channel == true && close_errored == false )
    {
        return OSCC_OK;
    }
    else
    {
        return OSCC_ERROR;
    }
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

    brake_cmd.pedal_command = (float) brake_position;

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

    throttle_cmd.torque_request = (float) throttle_position;

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

    steering_cmd.torque_command = (float) torque;

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


void oscc_update_status( int sig, siginfo_t *siginfo, void *context )
{
    struct can_frame rx_frame;
    memset( &rx_frame, 0, sizeof(rx_frame) );

    if ( global_oscc_can_socket >= 0 )
    {
        int oscc_can_bytes = read( global_oscc_can_socket, &rx_frame, CAN_MTU );

        while ( oscc_can_bytes > 0 )
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
                if ( obd_frame_callback != NULL && global_vehicle_can_socket < 0 )
                {
                    obd_frame_callback( &rx_frame );
                }
            }

            oscc_can_bytes = read( global_oscc_can_socket, &rx_frame, CAN_MTU );
        }
    }

    if ( global_vehicle_can_socket >= 0 )
    {
        int vehicle_can_bytes = read( global_vehicle_can_socket, &rx_frame, CAN_MTU );

        while( vehicle_can_bytes > 0 )
        {
            if ( obd_frame_callback != NULL )
            {
                obd_frame_callback( &rx_frame );
            }

            vehicle_can_bytes = read( global_vehicle_can_socket, &rx_frame, CAN_MTU );
        }
    }
}

oscc_result_t oscc_can_write( long id, void *msg, unsigned int dlc )
{
    oscc_result_t result = OSCC_ERROR;


    if ( global_oscc_can_socket >= 0 )
    {
        struct can_frame tx_frame;

        memset( &tx_frame, 0, sizeof(tx_frame) );
        tx_frame.can_id = id;
        tx_frame.can_dlc = dlc;
        memcpy( tx_frame.data, msg, dlc );

        int ret = write( global_oscc_can_socket, &tx_frame, sizeof(tx_frame) );

        if ( ret > 0 )
        {
            result = OSCC_OK;
        }
        else
        {
            perror( "Could not write to socket:" );
        }
    }


    return result;
}


oscc_result_t register_can_signal( )
{
    oscc_result_t result = OSCC_ERROR;

    struct sigaction sig;
    memset( &sig, 0, sizeof(sig) );

    sigemptyset( &sig.sa_mask );

    sig.sa_sigaction = oscc_update_status;

    sig.sa_flags = SA_SIGINFO | SA_RESTART;

    if( sigaction( SIGIO, &sig, NULL ) == 0 )
    {
        result = OSCC_OK;
    }

    return result;
}


oscc_result_t oscc_async_enable( int socket )
{
    oscc_result_t result = OSCC_ERROR;

    int ret = fcntl( socket, F_SETOWN, getpid( ) );

    if ( ret < 0 )
    {
        perror( "Setting owner process of socket failed:" );
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
            perror( "Setting nonblocking asynchronous socket I/O failed:" );

            result = OSCC_ERROR;
        }
    }


    return result;
}


oscc_result_t oscc_search_can( can_contains_s(*search_callback)( const char * ),
                               bool search_oscc )
{
    oscc_result_t result = OSCC_OK;

    if( search_callback == NULL )
    {
        result = OSCC_ERROR;
    }

    device_names_s dev_list =
    {
        .name = NULL,
        .size = 0
    };

    if( result == OSCC_OK )
    {
        result = construct_interfaces_list( &dev_list );
    }

    //temp_contents is the temporary storage of the current CAN channel
    //all_contents is the sum of all channels searched
    can_contains_s temp_contents;
    can_contains_s all_contents =
    {
        .is_oscc = !search_oscc,
        .has_vehicle = false
    };

    uint i;

    for( i = 0; i < dev_list.size; i++ )
    {
        if ( strstr( dev_list.name[i], "can") != NULL && result == OSCC_OK )
        {
            temp_contents = search_callback( dev_list.name[i] );

            all_contents.is_oscc |= temp_contents.is_oscc;

            all_contents.has_vehicle |= temp_contents.has_vehicle;

            //Leave the while loop if both requirements are met
            if( all_contents.is_oscc && all_contents.has_vehicle )
            {
                break;
            }
        }
    }

    if( dev_list.name != NULL )
    {
        result = clear_device_names( &dev_list );
    }

    return result;
}


can_contains_s auto_init_all_can( const char *can_channel )
{
    if ( can_channel == NULL )
    {
        can_contains_s contents =
        {
              .is_oscc = false,
              .has_vehicle = false
        };

        return contents;
    }

    can_contains_s contents = can_detection( can_channel );

    if( contents.is_oscc )
    {
        init_oscc_can( can_channel );
    }
    else if( contents.has_vehicle )
    {
        init_vehicle_can( can_channel );
    }

    return contents;
}


can_contains_s auto_init_vehicle_can( const char *can_channel )
{
    if ( can_channel == NULL )
    {
        can_contains_s contents =
        {
              .is_oscc = false,
              .has_vehicle = false
        };

        return contents;
    }

    can_contains_s contents = can_detection( can_channel );

    if( contents.has_vehicle )
    {
      init_vehicle_can( can_channel );
    }

    return contents;
}


oscc_result_t init_oscc_can( const char *can_channel )
{
    int result = OSCC_ERROR;

    if( can_channel != NULL )
    {
        printf( "Assigning OSCC CAN Channel to: %s\n", can_channel );

        global_oscc_can_socket = init_can_socket( can_channel, NULL );
    }

    if( can_channel != NULL && global_oscc_can_socket >= 0 )
    {
        result = OSCC_OK;
    }

    return result;
}


oscc_result_t init_vehicle_can( const char *can_channel )
{
    int result = OSCC_ERROR;

    if( can_channel != NULL )
    {
          printf( "Assigning Vehicle CAN Channel to: %s\n", can_channel );

          global_vehicle_can_socket = init_can_socket( can_channel, NULL );
    }

    if( can_channel != NULL && global_vehicle_can_socket >= 0 )
    {
        result = OSCC_OK;
    }

    return result;
}


int init_can_socket( const char *can_channel,
                     struct timeval *tv )
{
    if( can_channel == NULL )
    {
        return UNINITIALIZED_SOCKET;
    }

    int valid = UNINITIALIZED_SOCKET;
    int sock = UNINITIALIZED_SOCKET;
    struct ifreq ifr;
    memset( &ifr, 0, sizeof( ifr ) );

    sock = socket( PF_CAN, SOCK_RAW, CAN_RAW );

    if ( sock < 0 )
    {
        perror( "Opening CAN socket failed:" );
    }
    else
    {
        strncpy( ifr.ifr_name, can_channel, IFNAMSIZ );

        valid = ioctl( sock, SIOCGIFINDEX, &ifr );

        if ( valid < 0 )
        {
            perror( "Finding CAN index failed:" );
        }
    }

    //If a timeout has been specified set one here since it should be set before
    //the bind call
    if( valid >= 0 && tv != NULL )
    {
        valid = setsockopt( sock,
                            SOL_SOCKET,
                            SO_RCVTIMEO,
                            tv,
                            sizeof( struct timeval ) );

        if ( valid < 0 )
        {
            perror( "Setting timeout failed:" );
        }
    }

    if ( valid >= 0 )
    {
        struct sockaddr_can can_address;

        memset( &can_address, 0, sizeof( can_address ) );
        can_address.can_family = AF_CAN;
        can_address.can_ifindex = ifr.ifr_ifindex;

        valid = bind( sock,
                      (struct sockaddr *) &can_address,
                      sizeof( can_address ) );

        if ( valid < 0 )
        {
            perror( "Socket binding failed:" );
        }
    }

    // Clean up resources and close the connection if it's invalid.
    if( valid < 0 )
    {
        close( sock );
        sock = UNINITIALIZED_SOCKET;
    }

    return sock;
}


can_contains_s can_detection( const char *can_channel )
{
    if( can_channel == NULL )
    {
        can_contains_s detection =
        {
            .is_oscc = false,
            .has_vehicle = false
        };

        return detection;
    }

    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = CAN_MESSAGE_TIMEOUT;

    int sock = init_can_socket( can_channel, &timeout );

    vehicle_can_desc_s vehicle_detection =
    {
        .has_steering_angle = false,
        .has_brake_pressure = false,
        .has_wheel_speed = false
    };

    oscc_can_desc_s oscc_detection =
    {
        .has_accel_report = false,
        .has_steer_report = false,
        .has_brake_report = false
    };

    uint i = 0;

    for( i = 0; i < MAX_CAN_IDS; i++ )
    {
        struct can_frame rx_frame;
        memset( &rx_frame, 0, sizeof( rx_frame ) );
        int recv_bytes = 0;

        recv_bytes = read( sock, &rx_frame, sizeof( rx_frame ) );

        if( recv_bytes == CAN_MTU || recv_bytes == CANFD_MTU )
        {
            if ( (rx_frame.can_id < 0x100) &&
                 (rx_frame.data[0] == OSCC_MAGIC_BYTE_0) &&
                 (rx_frame.data[1] == OSCC_MAGIC_BYTE_1) )
            {
              oscc_detection.has_brake_report |=
                  ( (rx_frame.can_id == OSCC_BRAKE_REPORT_CAN_ID) );

              oscc_detection.has_steer_report |=
                  ( (rx_frame.can_id == OSCC_STEERING_REPORT_CAN_ID) );

              oscc_detection.has_accel_report |=
                  ( (rx_frame.can_id == OSCC_THROTTLE_REPORT_CAN_ID) );
            }

            vehicle_detection.has_brake_pressure |=
                ( rx_frame.can_id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID );

            vehicle_detection.has_steering_angle |=
                ( rx_frame.can_id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID );

            vehicle_detection.has_wheel_speed |=
                ( rx_frame.can_id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID );
        }
    }

    close( sock );

    can_contains_s detection =
    {
        .is_oscc = oscc_detection.has_brake_report &&
                   oscc_detection.has_steer_report &&
                   oscc_detection.has_accel_report,
        .has_vehicle = vehicle_detection.has_brake_pressure &&
                       vehicle_detection.has_steering_angle &&
                       vehicle_detection.has_wheel_speed
    };

    return detection;
}


oscc_result_t construct_interfaces_list( device_names_s * const names_ptr )
{
    FILE *file_handler;
    char buffer[512];
    oscc_result_t result = OSCC_OK;

    file_handler = fopen( "/proc/net/dev", "r" );
    if (!file_handler) {
        perror( "Cannot read: /proc/net/dev" );

        result = OSCC_ERROR;
    }

    if( result == OSCC_OK && names_ptr == NULL )
    {
        result = OSCC_ERROR;
    }

    int lines = 0;

    if( result == OSCC_OK )
    {
        while ( fgets( buffer, sizeof( buffer ), file_handler ) ) {
            ++lines;
        }

        names_ptr->name = (char**) malloc(lines * sizeof(char*));

        uint i;

        for ( i = 0; i < lines; i++ )
        {
            names_ptr->name[i] = (char*) malloc(IFNAMSIZ * sizeof(char));
        }

        rewind( file_handler );

        //Consume the first two lines since they are headers
        fgets( buffer, sizeof( buffer ), file_handler );
        fgets( buffer, sizeof( buffer ), file_handler );
    }

    char* socket_name = calloc( IFNAMSIZ, sizeof(char) );

    if( !socket_name )
    {
        result = OSCC_ERROR;
    }

    if( result == OSCC_OK )
    {
        uint size = 0;

        while ( size < lines && fgets( buffer, sizeof( buffer ), file_handler ) ) {

            result = get_device_name( buffer, socket_name );

            if( result == OSCC_OK )
            {
                strncpy( names_ptr->name[size], socket_name, IFNAMSIZ );

                size++;
            }
        }

        free( socket_name );

        names_ptr->size = size;
    }

    fclose( file_handler );

    return result;
}

oscc_result_t clear_device_names( device_names_s * const names_ptr )
{
    oscc_result_t result = OSCC_OK;

    if( names_ptr == NULL )
    {
        result = OSCC_ERROR;
    }
    else
    {
        uint i;

        for ( i = 0; i < names_ptr->size; i++ )
        {
            if( names_ptr->name[i] != NULL )
            {
                free( names_ptr->name[i] );

                names_ptr->name[i] = NULL;
            }

        }

        if( names_ptr->name != NULL )
        {
            free( names_ptr->name );

            names_ptr->name = NULL;
        }

    }

    return result;
}


oscc_result_t get_device_name( char * string, char * const name )
{
    oscc_result_t result = OSCC_OK;

    if( name == NULL || string == NULL )
    {
        result = OSCC_ERROR;
    }

    if( result == OSCC_OK )
    {
        size_t span = strcspn(string, ":");

        char temp_name[IFNAMSIZ];

        strncpy( temp_name, string, span );

        if( span <= IFNAMSIZ )
        {
            temp_name[span] = '\0';
        }

        size_t leading_spaces = strspn( temp_name, " " );

        if( leading_spaces != 0 )
        {
            char new_name[IFNAMSIZ];

            strncpy( name, temp_name + leading_spaces, span - leading_spaces + 1 );

            new_name[span - leading_spaces] = '\0';
        }
        else
        {
            strncpy( name, temp_name, span );
        }
    }

    return result;
}

static oscc_result_t get_wheel_speed(
    struct can_frame const * const frame,
    double * wheel_speed,
    const size_t offset)
{
    if((frame == NULL) || (wheel_speed == NULL))
    {
        return OSCC_ERROR;
    }

    if(frame->can_id != KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
    {
        return OSCC_ERROR;
    }

    uint16_t raw = ((frame->data[offset + 1] & 0x0F) << 8) | frame->data[offset];

    // 10^-1 precision, raw / 32.0
    *wheel_speed = (double)((int)((double)raw / 3.2) / 10.0);

    return OSCC_OK;
}

oscc_result_t get_wheel_speed_right_rear(
    struct can_frame const * const frame,
    double * wheel_speed_right_rear)
{
    size_t offset = 6;

    oscc_result_t ret = get_wheel_speed(frame, wheel_speed_right_rear, offset);

    return ret;
}


oscc_result_t get_wheel_speed_left_rear(
    struct can_frame const * const frame,
    double * wheel_speed_left_rear)
{
    size_t offset = 4;

    oscc_result_t ret = get_wheel_speed(frame, wheel_speed_left_rear, offset);

    return ret;
}


oscc_result_t get_wheel_speed_right_front(
    struct can_frame const * const frame,
    double * wheel_speed_right_front)
{
    size_t offset = 2;

    oscc_result_t ret = get_wheel_speed(frame, wheel_speed_right_front, offset);

    return ret;
}


oscc_result_t get_wheel_speed_left_front(
    struct can_frame const * const frame,
    double * wheel_speed_left_front)
{
    size_t offset = 0;

    oscc_result_t ret = get_wheel_speed(frame, wheel_speed_left_front, offset);

    return ret;
}


oscc_result_t get_steering_wheel_angle(
    struct can_frame const * const frame,
    double * steering_wheel_angle)
{
    if((frame == NULL) || (steering_wheel_angle == NULL))
    {
        return OSCC_ERROR;
    }

    if(frame->can_id != KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
    {
        return OSCC_ERROR;
    }

    int16_t raw = (frame->data[1] << 8) | frame->data[0];

    *steering_wheel_angle = -((double)raw * KIA_SOUL_OBD_STEERING_ANGLE_SCALAR);

    return OSCC_OK;
}


oscc_result_t get_brake_pressure(
    struct can_frame const * const frame,
    double * brake_pressure)
{
    if((frame == NULL) || (brake_pressure == NULL))
    {
        return OSCC_ERROR;
    }

    if(frame->can_id != KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID)
    {
        return OSCC_ERROR;
    }

#ifdef KIA_NIRO
    double scale = 40.0;
    uint16_t raw = ((frame->data[4] & 0x0F) << 8) | frame->data[3];
#else
    double scale = 10.0;
    uint16_t raw = ((frame->data[5] & 0x0F) << 8) | frame->data[4];
#endif

    *brake_pressure = (double)raw / scale;

    return OSCC_OK;
}
