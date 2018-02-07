#include <errno.h>
#include <fcntl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <unistd.h>

#include "oscc.h"
#include "internal/oscc.h"


static int oscc_can_socket = -1;
static int vehicle_can_socket = -1;
static uint64_t dummy_counter = 0;

oscc_result_t oscc_open()
{
  oscc_result_t result = OSCC_ERROR;

  result = register_can_signal();

  if( retsult != OSCC_ERROR )
  {
    static struct dev_name *dev_list, temp_ptr;
  	dev_list = malloc(sizeof(struct dev_name));

  	ret = construct_interfaces_list(dev_list);

    temp_ptr = dev_list;

    do
    {
      if (strstr(temp_ptr->name,"can") != NULL)
      {
        printf("%s\n", temp_ptr->name);
        //Check and assign if OSCC CAN
        //Check and assign if Vehicle CAN
      }

      temp_ptr = temp_ptr->next;
    }while(temp_ptr != dev_list);
  }

}

oscc_result_t oscc_open( unsigned int channel )
{
    oscc_result_t result = OSCC_ERROR;

    result = register_can_signal();

    if( result != OSCC_ERROR )
    {
        char can_string_buffer[16];

        snprintf( can_string_buffer, 16, "can%u", channel );

        printf( "Opening CAN channel: %s\n", can_string_buffer );

        result = oscc_init_can( can_string_buffer );
    }

    //If no vehicle CAN through OSCC add CAN detection here for second channel...
    //Warn if no vehicle CAN is found.

    return result;
}

oscc_result_t oscc_open( const char * const oscc_channel,
                         const char * const vehicle_channel )
{
    oscc_result_t result = OSCC_ERROR;

    result = register_can_signal();

    if( result != OSCC_ERROR )
    {
        char can_string_buffer[16];

        snprintf( can_string_buffer, 16, oscc_channel );

        printf( "Opening CAN channel: %s\n", can_string_buffer );

        result = oscc_init_can( can_string_buffer );
    }

    if(result != OSCC_ERROR)
    {
        char veh_can_string_buffer[16];

        snprintf( veh_can_string_buffer, 16, vehicle_can );

        printf( "Opening CAN channel: %s\n", veh_can_string_buffer );

        result = vehicle_init_can( veh_can_string_buffer );
    }

    return result;
};

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
            printf( "Could not write to socket: %s\n", strerror(errno) );
        }
    }


    return result;
}

oscc_result_t oscc_async_enable( int socket )
{
    oscc_result_t result = OSCC_ERROR;


    int ret = fcntl( socket, F_SETOWN, (pid_t) syscall (SYS_gettid) );

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

oscc_result_t oscc_init_can( const char *can_channel )
{
    int result = OSCC_ERROR;
    int ret = -1;

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
            oscc_can_socket = sock;
        }
    }


    return result;
}

oscc_result_t vehicle_init_can( const char *can_channel )
{
    int result = OSCC_ERROR;
    int ret = -1;

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

    struct can_filter rfilter[4];

    rfilter[0].can_id   = KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID;
    rfilter[0].can_mask = CAN_SFF_MASK;
    rfilter[1].can_id   = KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID;
    rfilter[1].can_mask = CAN_SFF_MASK;
    rfilter[2].can_id   = KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID;
    rfilter[2].can_mask = CAN_SFF_MASK;
    rfilter[2].can_id   = KIA_SOUL_OBD_VEHICLE_SPEED_CAN_ID;
    rfilter[2].can_mask = CAN_SFF_MASK;


    setsockopt(sock, SOL_CAN_RAW, CAN_RAW_FILTER, &rfilter, sizeof(rfilter));


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
        vehicle_can_socket = sock;
    }


    return result;
}

//Might want to put these CAN detection functions in their own library...

char * get_dev_name(char *string)
{
	size_t span = strcspn(string, ":");
	static char temp_name[IFNAMSIZ];
	strncpy(temp_name, string, span);
	size_t leading_spaces = strspn(temp_name, " ");

	if(leading_spaces == 0)
	{
		return temp_name;
	}

	static char new_name[IFNAMSIZ];

	strncpy(new_name, temp_name + leading_spaces, span - leading_spaces + 1);

	return new_name;
}

oscc_result_t add_dev_name(const char * const name, struct dev_name * const list_ptr)
{
	if(list_ptr == NULL){
		fprintf(stderr,
						"dev list is uninitialized (%s)\n",
						strerror(errno));
		return -1;
	}

	if(strlen(list_ptr->name) != 0)
	{
		struct dev_name * old_tail;
		struct dev_name * new_name = malloc(sizeof(struct dev_name));
		strncpy(new_name->name, name, IFNAMSIZ);

		if(list_ptr->next == list_ptr){
			list_ptr->next = new_name;
		}
		old_tail = list_ptr->prev;
		old_tail->next = new_name;
		new_name->prev = old_tail;
		new_name->next = list_ptr;
		list_ptr->prev = new_name;
	}
	else{
		strncpy(list_ptr->name, name, IFNAMSIZ);
		list_ptr->prev = list_ptr;
		list_ptr->next = list_ptr;
	}

	return OSCC_OK;
}

oscc_result_t construct_interfaces_list(struct dev_name * const list_ptr)
{
	FILE *fh;
  char buffer[512];
  oscc_result_t ret = OSCC_ERROR;

  fh = fopen("/proc/net/dev", "r");
  if (!fh) {
	  fprintf(stderr,
			      "Cannot read: /proc/net/dev (%s).\n",
						strerror(errno));
	  return -2;
	}

	//Consume the first two lines since they are headers
  fgets(buffer, sizeof buffer, fh);
  fgets(buffer, sizeof buffer, fh);

	while (fgets(buffer, sizeof buffer, fh)) {
		char *socket_name;
		socket_name = get_dev_name(buffer);
		ret = add_dev_name(socket_name, list_ptr);
	}

	return ret;
}
