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
#include "vehicles/vehicles.h"
#include "dtc.h"
#include "oscc.h"

#define m_constrain(amt, low, high) ((amt) < (low) ? (low) : ((amt) > (high) ? (high) : (amt)))

static int can_socket;

static oscc_brake_command_s brake_cmd;
static oscc_throttle_command_s throttle_cmd;
static oscc_steering_command_s steering_cmd;

static void (*steering_report_callback)(oscc_steering_report_s *report);
static void (*brake_report_callback)(oscc_brake_report_s *report);
static void (*throttle_report_callback)(oscc_throttle_report_s *report);
static void (*fault_report_callback)(oscc_fault_report_s *report);
static void (*obd_frame_callback)(long id, unsigned char *data);

static oscc_error_t oscc_init_can(const char *can_channel);
static oscc_error_t oscc_can_write(long id, void *msg, unsigned int dlc);
static oscc_error_t oscc_async_enable(int socket);
static oscc_error_t oscc_enable_brakes();
static oscc_error_t oscc_enable_throttle();
static oscc_error_t oscc_enable_steering();
static oscc_error_t oscc_disable_brakes();
static oscc_error_t oscc_disable_throttle();
static oscc_error_t oscc_disable_steering();
static void oscc_update_status();

oscc_error_t oscc_open(unsigned int channel)
{
    oscc_error_t ret = OSCC_ERROR;

    char buffer[16];

    snprintf(buffer, 16, "can%1d", channel);

    printf("Opening CAN channel: %s\n", buffer);

    ret = oscc_init_can(buffer);

    return ret;
}

oscc_error_t oscc_close(unsigned int channel)
{
    oscc_error_t ret = OSCC_ERROR;

    int result = close(can_socket);

    if (result > 0)
    {
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_enable()
{
    oscc_error_t ret = oscc_enable_brakes();

    if (ret == OSCC_OK)
    {
        ret = oscc_enable_throttle();

        if (ret == OSCC_OK)
        {
            ret = oscc_enable_steering();
        }
    }

    return ret;
}

oscc_error_t oscc_disable()
{
    oscc_error_t ret = oscc_disable_brakes();

    if (ret == OSCC_OK)
    {
        ret = oscc_disable_throttle();

        if (ret == OSCC_OK)
        {
            ret = oscc_disable_steering();
        }
    }

    return ret;
}

oscc_error_t oscc_publish_brake_position(double brake_position)
{
    oscc_error_t ret = OSCC_ERROR;

    // use normalized position to scale between known limits
    // use that to calculate spoof values
    const double scaled_position = (double) m_constrain (
            brake_position * MAXIMUM_BRAKE_COMMAND,
            MINIMUM_BRAKE_COMMAND,
            MAXIMUM_BRAKE_COMMAND );

    brake_cmd.magic = ( uint16_t ) OSCC_MAGIC;
    brake_cmd.pedal_command = ( uint16_t ) BRAKE_POSITION_TO_PEDAL( scaled_position );

    ret = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                    (void *) &brake_cmd,
                                    sizeof( brake_cmd ) );

    return ret;
}


oscc_error_t oscc_publish_brake_pressure( double brake_pressure )
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.magic = ( uint16_t ) OSCC_MAGIC;
    brake_cmd.pedal_command = ( uint16_t ) BRAKE_PRESSURE_TO_PEDAL( brake_pressure );

    ret = oscc_can_write(OSCC_BRAKE_COMMAND_CAN_ID,
                         (void *)&brake_cmd,
                         sizeof(brake_cmd));

    return ret;
}

oscc_error_t oscc_publish_throttle_position(double throttle_position)
{
    oscc_error_t ret = OSCC_ERROR;

    // use normalized throttle position to scale between known limits
    // use that to calculate spoof values

    throttle_cmd.magic = ( uint16_t ) OSCC_MAGIC;
    throttle_cmd.spoof_value_low = ( uint16_t) THROTTLE_POSITION_TO_SPOOF_LOW( throttle_position );
    throttle_cmd.spoof_value_high = ( uint16_t ) THROTTLE_POSITION_TO_SPOOF_HIGH( throttle_position );

    ret = oscc_can_write(OSCC_THROTTLE_COMMAND_CAN_ID,
                         (void *)&throttle_cmd,
                         sizeof(throttle_cmd));

    return ret;
}

oscc_error_t oscc_publish_steering_torque(double normalized_torque)
{
    oscc_error_t ret = OSCC_ERROR;

    double torque = normalized_torque * STEERING_TORQUE_MAX;

    steering_cmd.magic = ( uint16_t ) OSCC_MAGIC;
    steering_cmd.spoof_value_low = ( int16_t ) STEERING_TORQUE_TO_SPOOF_LOW( torque );
    steering_cmd.spoof_value_high = ( int16_t ) STEERING_TORQUE_TO_SPOOF_HIGH( torque );

    ret = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                    (void *) &steering_cmd,
                                    sizeof( steering_cmd ) );

    return ret;
}

oscc_error_t oscc_subscribe_to_brake_reports(void (*callback)(oscc_brake_report_s *report))
{
    oscc_error_t ret = OSCC_ERROR;

    if (callback != NULL)
    {
        brake_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_throttle_reports(void (*callback)(oscc_throttle_report_s *report))
{
    oscc_error_t ret = OSCC_ERROR;

    if (callback != NULL)
    {
        throttle_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_steering_reports(void (*callback)(oscc_steering_report_s *report))
{
    oscc_error_t ret = OSCC_ERROR;

    if (callback != NULL)
    {
        steering_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_fault_reports(void (*callback)(oscc_fault_report_s *report))
{
    oscc_error_t ret = OSCC_ERROR;

    if (callback != NULL)
    {
        fault_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_obd_messages(void (*callback)(long id, unsigned char *data))
{
    oscc_error_t ret = OSCC_ERROR;

    if (callback != NULL)
    {
        obd_frame_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

static oscc_error_t oscc_enable_brakes()
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.enable = 1;

    ret = oscc_publish_brake_position(0);

    return ret;
}

static oscc_error_t oscc_enable_throttle()
{
    oscc_error_t ret = OSCC_ERROR;

    throttle_cmd.enable = 1;

    ret = oscc_publish_throttle_position(0);

    return ret;
}

static oscc_error_t oscc_enable_steering()
{
    oscc_error_t ret = OSCC_ERROR;

    steering_cmd.enable = 1;

    ret = oscc_publish_steering_torque(0);

    return ret;
}

static oscc_error_t oscc_disable_brakes()
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.enable = 0;

    ret = oscc_publish_brake_position(0);

    return ret;
}

static oscc_error_t oscc_disable_throttle()
{
    oscc_error_t ret = OSCC_ERROR;

    throttle_cmd.enable = 0;

    ret = oscc_publish_throttle_position(0);

    return ret;
}

static oscc_error_t oscc_disable_steering()
{
    oscc_error_t ret = OSCC_ERROR;

    steering_cmd.enable = 0;

    ret = oscc_publish_steering_torque(0);

    return ret;
}

static void oscc_update_status()
{
    struct can_frame rx_frame;

    int result = read(can_socket, &rx_frame, CAN_MTU);

    while (result > 0)
    {
        if (rx_frame.can_id == OSCC_STEERING_REPORT_CAN_ID)
        {
            oscc_steering_report_s *steering_report =
                (oscc_steering_report_s *)rx_frame.data;

            if (steering_report_callback != NULL)
            {
                steering_report_callback(steering_report);
            }
        }
        else if (rx_frame.can_id == OSCC_THROTTLE_REPORT_CAN_ID)
        {  
            oscc_throttle_report_s *throttle_report =
                (oscc_throttle_report_s *)rx_frame.data;

            if (throttle_report_callback != NULL)
            {
                throttle_report_callback(throttle_report);
            }
        }
        else if (rx_frame.can_id == OSCC_BRAKE_REPORT_CAN_ID)
        {
            oscc_brake_report_s *brake_report =
                (oscc_brake_report_s *)rx_frame.data;

            if (brake_report_callback != NULL)
            {
                brake_report_callback(brake_report);
            }
        }
        else if (rx_frame.can_id == OSCC_FAULT_REPORT_CAN_ID)
        {
            oscc_fault_report_s *fault_report =
                (oscc_fault_report_s *)rx_frame.data;

            if (fault_report_callback != NULL)
            {
                fault_report_callback(fault_report);
            }
        }
        else
        {
            if (obd_frame_callback != NULL)
            {
                obd_frame_callback(rx_frame.can_id, rx_frame.data);
            }
        }

        result = read(can_socket, &rx_frame, CAN_MTU);
    }
}

static oscc_error_t oscc_can_write(long id, void *msg, unsigned int dlc)
{
    oscc_error_t ret = OSCC_ERROR;

    struct can_frame tx_frame;

    tx_frame.can_id = id;
    tx_frame.can_dlc = dlc;
    memcpy(tx_frame.data, msg, dlc);

    int result = write(can_socket, &tx_frame, sizeof(tx_frame));

    if (result > 0)
    {
        ret = OSCC_OK;
    }

    return ret;
}

static oscc_error_t oscc_async_enable(int socket)
{
    oscc_error_t ret = OSCC_ERROR;

    if (socket >= 0)
    {
        int state = fcntl(socket, F_GETFL, 0);

        if (state >= 0)
        {
            state |= O_ASYNC;

            int result = fcntl(socket, F_SETFL, state );

            if (result >= 0)
            {
                fcntl(socket, F_SETOWN, getpid());
                ret = OSCC_OK;
            }
        }
    }

    return ret;
}

static void quit_handler()
{
    exit(0);
}

static oscc_error_t oscc_init_can(const char *can_channel)
{
    int ret = OSCC_OK;

    int s = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (s < 0)
    {
        printf("opening can socket failed\n");

        ret = OSCC_ERROR;
    }

    int status;

    struct ifreq ifr;

    if (ret != OSCC_ERROR)
    {
        strncpy(ifr.ifr_name, can_channel, IFNAMSIZ);

        status = ioctl(s, SIOCGIFINDEX, &ifr);

        if (status < 0)
        {
            printf("finding can index failed\n");

            ret = OSCC_ERROR;
        }
    }

    if (ret != OSCC_ERROR)
    {
        struct sockaddr_can can_address;

        can_address.can_family = AF_CAN;
        can_address.can_ifindex = ifr.ifr_ifindex;

        status = bind(s,
                      (struct sockaddr *)&can_address,
                      sizeof(can_address));

        if (status < 0)
        {
            printf("socket binding failed\n");

            ret = OSCC_ERROR;
        }
    }

    if (ret != OSCC_ERROR)
    {
        status = oscc_async_enable(s);

        if (status != OSCC_OK)
        {
            printf("async enable failed\n");

            ret = OSCC_ERROR;
        }
    }

    if (ret != OSCC_ERROR)
    {
        signal(SIGIO, &oscc_update_status);
        signal(SIGINT, &quit_handler);

        can_socket = s;

        ret = OSCC_OK;
    }

    return ret;
}
