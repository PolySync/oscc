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


static int oscc_can_socket = -1;
static int vehicle_can_socket = -1;


oscc_result_t oscc_init()
{
    oscc_result_t result = OSCC_ERROR;

    result = oscc_search_can( &auto_init_all_can, true );

    if( result != OSCC_ERROR )
    {
        result = register_can_signal();
    }

    if ( oscc_can_socket > 0 )
    {
        result = oscc_async_enable( oscc_can_socket );
    }
    else
    {
        printf( "Error could not find OSCC CAN signal\n" );
        result = OSCC_ERROR;
    }

    if ( result == OSCC_OK && vehicle_can_socket > 0 )
    {
        oscc_async_enable( vehicle_can_socket );
    }

    return result;
}

oscc_result_t oscc_open( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;

    struct can_contains channel_contents;

    char can_string_buffer[16];

    snprintf( can_string_buffer, 16, "can%u", channel );

    channel_contents = can_detection( can_string_buffer );

    if( result != OSCC_ERROR && !channel_contents.has_vehicle )
    {
        int vehicle_ret = OSCC_ERROR;

        vehicle_ret = oscc_search_can( &auto_init_vehicle_can, false );

        if( (vehicle_can_socket < 0) || (vehicle_ret != OSCC_OK) )
        {
            printf( "Warning Vehicle CAN was not found.\n" );
        }
    }

    if( result != OSCC_ERROR)
    {
        result = init_oscc_can( can_string_buffer );
    }

    result = register_can_signal();

    if ( result != OSCC_ERROR && oscc_can_socket >= 0 )
    {
        result = oscc_async_enable( oscc_can_socket );
    }
    else
    {
        printf( "Error could not find OSCC CAN signal.\n" );
    }

    if ( result != OSCC_ERROR && vehicle_can_socket >= 0 )
    {
        oscc_async_enable( vehicle_can_socket );
    }

    return result;
}


oscc_result_t oscc_close( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;

    if( oscc_can_socket != -1 )
    {
        int result = close( oscc_can_socket );

        if ( result > 0 )
        {
            result = OSCC_OK;
        }
    }

    if( vehicle_can_socket != -1 )
    {
        int result = close( vehicle_can_socket );

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

    if ( oscc_can_socket != -1 )
    {
        int ret = read( oscc_can_socket, &rx_frame, CAN_MTU );

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
                if ( obd_frame_callback != NULL && vehicle_can_socket < 0 )
                {
                    obd_frame_callback( &rx_frame );
                }
            }

            ret = read( oscc_can_socket, &rx_frame, CAN_MTU );
        }
    }

    if ( vehicle_can_socket != -1 )
    {
      int veh_ret = read( vehicle_can_socket, &rx_frame, CAN_MTU );

      while( veh_ret > 0 )
      {
          if ( obd_frame_callback != NULL )
          {
              obd_frame_callback( &rx_frame );
          }

          veh_ret = read( vehicle_can_socket, &rx_frame, CAN_MTU );
      }
    }
}

oscc_result_t oscc_can_write( long id, void *msg, unsigned int dlc )
{
    oscc_result_t result = OSCC_ERROR;


    if ( oscc_can_socket != -1 )
    {
        struct can_frame tx_frame;

        memset( &tx_frame, 0, sizeof(tx_frame) );
        tx_frame.can_id = id;
        tx_frame.can_dlc = dlc;
        memcpy( tx_frame.data, msg, dlc );

        int ret = write( oscc_can_socket, &tx_frame, sizeof(tx_frame) );

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


oscc_result_t register_can_signal()
{
  int result = OSCC_ERROR;
  struct sigaction sig;
  memset( &sig, 0, sizeof(sig) );
  sigemptyset( &sig.sa_mask );
  sig.sa_sigaction = oscc_update_status;
  sig.sa_flags = SA_SIGINFO;
  if( sigaction( SIGIO, &sig, NULL ) == 0 )
  {
    result = OSCC_OK;
  }

  return result;
}


oscc_result_t oscc_async_enable( int socket )
{
    oscc_result_t result = OSCC_ERROR;


    int ret = fcntl( socket, F_SETOWN, (pid_t) syscall (SYS_gettid) );

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


oscc_result_t oscc_search_can( struct can_contains(*search_callback)( const char * ),
                               bool search_oscc )
{
    oscc_result_t result = OSCC_ERROR;

    static struct device_name *dev_list, *temp_ptr;
    dev_list = malloc( sizeof( struct device_name ) );

    result = construct_interfaces_list( dev_list );

    //temp_contents is the temporary storage of the current CAN channel
    //all_contents is the sum of all channels searched
    struct can_contains temp_contents, all_contents;

    //Set the all_contents based search_oscc boolean in function call
    all_contents.is_oscc = !search_oscc;
    all_contents.has_vehicle = false;

    temp_ptr = dev_list;

    do
    {
        if ( strstr( temp_ptr->name, "can" ) != NULL )
        {
            temp_contents = search_callback( temp_ptr->name );

            all_contents.is_oscc |= temp_contents.is_oscc;

            all_contents.has_vehicle |= temp_contents.has_vehicle;

            //Leave the while loop if both requirements are met
            if( all_contents.is_oscc && all_contents.has_vehicle )
            {
                break;
            }
        }

        temp_ptr = temp_ptr->next;
    }while( temp_ptr != dev_list );

    return result;
}


struct can_contains auto_init_all_can( const char *can_channel )
{
    struct can_contains contents = can_detection( can_channel );

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


struct can_contains auto_init_vehicle_can( const char *can_channel )
{
    struct can_contains contents = can_detection( can_channel );

    if( contents.has_vehicle )
    {
      init_vehicle_can( can_channel );
    }

    return contents;
}


oscc_result_t init_oscc_can( const char *can_channel )
{
    int result = OSCC_ERROR;

    printf( "Assigning OSCC CAN Channel to: %s\n", can_channel );

    oscc_can_socket = init_can_socket( can_channel, NULL, NULL );

    if( oscc_can_socket >= 0 )
    {
        result = OSCC_OK;
    }

    return result;
}


oscc_result_t init_vehicle_can( const char *can_channel )
{
    int result = OSCC_ERROR;

    printf( "Assigning Vehicle CAN Channel to: %s\n", can_channel );

    struct can_filter rfilter[4];

    rfilter[0].can_id   = KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID;
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id   = KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID;
    rfilter[1].can_mask = CAN_SFF_MASK;
    rfilter[2].can_id   = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;
    rfilter[2].can_mask = CAN_SFF_MASK;

    vehicle_can_socket = init_can_socket( can_channel, NULL, rfilter );

    if(vehicle_can_socket >= 0)
    {
        result = OSCC_OK;
    }

    return result;
}


int init_can_socket( const char *can_channel,
                     struct timeval *tv,
                     struct can_filter *filter )
{
    int valid = -1;
    int sock = -1;
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

    //If a filter has been specified set one here
    if( (valid >= 0) && (filter != NULL) )
    {
        int ret = setsockopt( sock,
                    SOL_CAN_RAW,
                    CAN_RAW_FILTER,
                    &filter,
                    sizeof( filter ) );

        if ( valid < 0 )
        {
            perror( "Setting filter failed:" );
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
      sock = -1;
    }

    return sock;
}


struct can_contains can_detection( const char *can_channel )
{
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 75000;

    int sock = init_can_socket( can_channel, &timeout, NULL );

    int i = 0;

    struct vehicle_can_desc vehicle_detection =
    {
      .has_steering_angle = false,
      .has_brake_pressure = false,
      .has_wheel_speed = false
    };

    struct oscc_can_desc oscc_detection =
    {
      .has_torqe_report = false,
      .has_steer_report = false,
      .has_brake_report = false
    };

    for( i = 0; i < MAX_CAN_IDS; i++ )
    {
        struct can_frame rx_frame;
        memset( &rx_frame, 0, sizeof( rx_frame ) );
        int recv_bytes = 0;

        recv_bytes = read( sock, &rx_frame, sizeof( rx_frame ) );

        switch (recv_bytes) {
          case CAN_MTU:
          case CANFD_MTU:
              oscc_detection.has_brake_report |=
                  ( (rx_frame.can_id == OSCC_BRAKE_REPORT_CAN_ID) &&
                    (rx_frame.data[0] == OSCC_MAGIC_BYTE_0) &&
                    (rx_frame.data[1] == OSCC_MAGIC_BYTE_1) );

              oscc_detection.has_steer_report |=
                  ( (rx_frame.can_id == OSCC_STEERING_REPORT_CAN_ID) &&
                    (rx_frame.data[0] == OSCC_MAGIC_BYTE_0) &&
                    (rx_frame.data[1] == OSCC_MAGIC_BYTE_1) );

              oscc_detection.has_torqe_report |=
                  ( (rx_frame.can_id == OSCC_THROTTLE_REPORT_CAN_ID) &&
                    (rx_frame.data[0] == OSCC_MAGIC_BYTE_0) &&
                    (rx_frame.data[1] == OSCC_MAGIC_BYTE_1) );

              vehicle_detection.has_brake_pressure |=
                  ( rx_frame.can_id == KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID );

              vehicle_detection.has_steering_angle |=
                  ( rx_frame.can_id == KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID );

              vehicle_detection.has_wheel_speed |=
                  ( rx_frame.can_id == KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID );
          case -1:
              if( EINTR == errno )
              {
                  continue;
              }
          default:
              continue;
        }
    }

    close( sock );

    struct can_contains detection =
    {
      .is_oscc = oscc_detection.has_brake_report &&
                 oscc_detection.has_steer_report &&
                 oscc_detection.has_torqe_report,
      .has_vehicle = vehicle_detection.has_brake_pressure &&
                     vehicle_detection.has_steering_angle &&
                     vehicle_detection.has_wheel_speed
    };

    return detection;
}


oscc_result_t construct_interfaces_list( struct device_name * const list_ptr )
{
    FILE *fh;
    char buffer[512];
    oscc_result_t ret = OSCC_ERROR;

    fh = fopen( "/proc/net/dev", "r" );
    if (!fh) {
        perror( "Cannot read: /proc/net/dev" );

        return -2;
    }

    //Consume the first two lines since they are headers
    fgets( buffer, sizeof buffer, fh );
    fgets( buffer, sizeof buffer, fh );

    while ( fgets( buffer, sizeof buffer, fh ) ) {
        char *socket_name;
        socket_name = get_device_name( buffer );
        ret = add_device_name( socket_name, list_ptr );
    }

    return ret;
}


char * get_device_name( char *string )
{
    size_t span = strcspn(string, ":");
    static char temp_name[IFNAMSIZ];
    strncpy(temp_name, string, span);
    temp_name[span] = '\0';
    size_t leading_spaces = strspn(temp_name, " ");

    if(leading_spaces == 0)
    {
        return temp_name;
    }

    static char new_name[IFNAMSIZ];

    strncpy(new_name, temp_name + leading_spaces, span - leading_spaces + 1);

    new_name[span - leading_spaces] = '\0';

    return new_name;
}

oscc_result_t add_device_name( const char * const name,
                               struct device_name * const list_ptr )
{
    if( list_ptr == NULL )
    {
        perror( "list is uninitialized" );
        return -1;
    }

    if( strlen( list_ptr->name ) != 0 )
    {
        struct device_name * old_tail;
        struct device_name * new_name = malloc( sizeof( struct device_name ) );
        strncpy( new_name->name, name, IFNAMSIZ );

        if( list_ptr->next == list_ptr ){
            list_ptr->next = new_name;
        }
        old_tail = list_ptr->prev;
        old_tail->next = new_name;
        new_name->prev = old_tail;
        new_name->next = list_ptr;
        list_ptr->prev = new_name;
    }
    else{
        strncpy( list_ptr->name, name, IFNAMSIZ );
        list_ptr->prev = list_ptr;
        list_ptr->next = list_ptr;
    }

    return OSCC_OK;
}
