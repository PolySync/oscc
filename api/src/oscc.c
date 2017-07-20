#include <stdio.h>
#include <stdlib.h>
#include <canlib.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "dtc.h"
#include "oscc.h"

#pragma pack(push)
#pragma pack(1)

typedef struct
{
    oscc_brake_command_s brake_cmd;
    oscc_throttle_command_s throttle_cmd;
    oscc_steering_command_s steering_cmd;

    canHandle can_handle;
    int can_channel;
} oscc_command_data_s;

// restore alignment
#pragma pack(pop)

static oscc_command_data_s oscc_data;
static oscc_command_data_s* oscc = NULL;

static void( *steering_report_callback )( oscc_steering_report_s *report );
static void( *brake_report_callback )( oscc_brake_report_s *report );
static void( *throttle_report_callback )( oscc_throttle_report_s *report );
static void( *obd_frame_callback )( long id, unsigned char * data );

static bool oscc_check_for_operator_override(
    long can_id,
    unsigned char * buffer );
static void oscc_check_for_obd_timeout(
    long can_id,
    unsigned char * buffer );
static void oscc_check_for_invalid_sensor_value(
    long can_id,
    unsigned char * buffer );

static oscc_error_t oscc_disable_brakes( );
static oscc_error_t oscc_disable_throttle( );
static oscc_error_t oscc_disable_steering( );
static void oscc_update_status( canNotifyData *data );
static oscc_error_t oscc_can_write( long id, void* msg, unsigned int dlc );
static oscc_error_t oscc_init_can( int channel );

oscc_error_t oscc_open( unsigned int channel )
{
    oscc_error_t ret = OSCC_ERROR;

    ret = oscc_init_can( channel );

    if ( ret == OSCC_OK )
    {
        oscc = &oscc_data;
    }

    return ret;
}

oscc_error_t oscc_close( unsigned int channel )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        canStatus status = canWriteSync( oscc->can_handle, 1000 );

        if ( status == canOK )
        {
            status = canClose( oscc->can_handle );
        }

        if ( status == canOK )
        {
            ret = OSCC_OK;
        }
    }

    oscc = NULL;

    return ret;
}

oscc_error_t oscc_enable( )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        ret = OSCC_OK;

        oscc->brake_cmd.enable = 1;
        oscc->throttle_cmd.enable = 1;
        oscc->steering_cmd.enable = 1;
    }

    return ret;
}

oscc_error_t oscc_disable( )
{
    oscc_error_t ret = oscc_disable_brakes( );

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

oscc_error_t oscc_publish_brake_position( unsigned int brake_position )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.pedal_command = ( uint16_t )brake_position;

        // MATHHHHHHHHHHH

        ret = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                      (void *) &oscc->brake_cmd,
                                      sizeof( oscc->brake_cmd ) );
    }

    return ret;
}

oscc_error_t oscc_publish_brake_pressure( double brake_pressure )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.pedal_command = ( uint16_t )brake_pressure;

        // MATHHHHHHHHHHH

        ret = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                      (void *) &oscc->brake_cmd,
                                      sizeof( oscc->brake_cmd ) );
    }

    return ret;
}

oscc_error_t oscc_publish_throttle_position( unsigned int throttle_position )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        // MATHHHHHHHHHHH

        oscc->throttle_cmd.spoof_value_low = ( uint16_t )throttle_position;
        oscc->throttle_cmd.spoof_value_high = ( uint16_t )throttle_position;

        ret = oscc_can_write( OSCC_THROTTLE_COMMAND_CAN_ID,
                                      (void *) &oscc->throttle_cmd,
                                      sizeof( oscc->throttle_cmd ) );
    }

    return ret;
}

oscc_error_t oscc_publish_steering_angle( double angle )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        // MATHHHHHHHHHHH

        oscc->steering_cmd.spoof_value_low = ( int16_t )angle;
        oscc->steering_cmd.spoof_value_high = ( int16_t )angle;

        ret = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                      (void *) &oscc->steering_cmd,
                                      sizeof( oscc->steering_cmd ) );
    }

    return ret;
}

oscc_error_t oscc_publish_steering_torque( double torque )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        // MATHHHHHHHHHHH

        oscc->steering_cmd.spoof_value_low = ( int16_t )torque;
        oscc->steering_cmd.spoof_value_high = ( int16_t )torque;

        ret = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                      (void *) &oscc->steering_cmd,
                                      sizeof( oscc->steering_cmd ) );
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_brake_reports( void( *callback )( oscc_brake_report_s *report ) )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        brake_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_throttle_reports( void( *callback )( oscc_throttle_report_s *report ) )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        throttle_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_steering_reports( void( *callback )( oscc_steering_report_s *report ) )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        steering_report_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

oscc_error_t oscc_subscribe_to_obd_messages( void( *callback )( long id, unsigned char * data ) )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        obd_frame_callback = callback;
        ret = OSCC_OK;
    }

    return ret;
}

static oscc_error_t oscc_disable_brakes( )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.enable = 0;

        ret = oscc_publish_brake_position( 0 );
    }

    return ret;
}

static oscc_error_t oscc_disable_throttle( )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        oscc->throttle_cmd.enable = 0;

        ret = oscc_publish_throttle_position( 0 );
    }

    return ret;
}

static oscc_error_t oscc_disable_steering( )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        oscc->steering_cmd.enable = 0;

        ret = oscc_publish_steering_angle( 0 );
    }

    return ret;
}

static void oscc_update_status( canNotifyData *data )
{
    {
        long can_id;
        unsigned int msg_dlc;
        unsigned int msg_flag;
        unsigned long tstamp;
        unsigned char buffer[ 8 ];

        canStatus can_status = canRead( oscc->can_handle,
                                        &can_id,
                                        buffer,
                                        &msg_dlc,
                                        &msg_flag,
                                        &tstamp );

        while ( can_status == canOK )
        {
            if ( can_id == OSCC_STEERING_REPORT_CAN_ID ) {
                oscc_steering_report_s* steering_report =
                    ( oscc_steering_report_s* )buffer;

                if (steering_report_callback != NULL)
                {
                    steering_report_callback(steering_report);
                }
            }
            else if ( can_id == OSCC_THROTTLE_REPORT_CAN_ID ) {
                oscc_throttle_report_s* throttle_report =
                    ( oscc_throttle_report_s* )buffer;

                if (throttle_report_callback != NULL)
                {
                    throttle_report_callback(throttle_report);
                }
            }
            else if ( can_id == OSCC_BRAKE_REPORT_CAN_ID ) {
                oscc_brake_report_s* brake_report =
                    ( oscc_brake_report_s* )buffer;

                if (brake_report_callback != NULL)
                {
                    brake_report_callback(brake_report);
                }
            }
            else
            {
                printf("obd frame rec'vd\n");
                if ( obd_frame_callback != NULL )
                {
                    obd_frame_callback( can_id, buffer );
                }
            }

            can_status = canRead( oscc->can_handle,
                                        &can_id,
                                        buffer,
                                        &msg_dlc,
                                        &msg_flag,
                                        &tstamp );
        }
    }
}

static oscc_error_t oscc_can_write( long id, void* msg, unsigned int dlc )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( oscc != NULL )
    {
        canStatus status = canWrite( oscc->can_handle, id, msg, dlc, 0 );

        if ( status == canOK )
        {
            ret = OSCC_OK;
        }
    }

    return ret;
}

static oscc_error_t oscc_init_can( int channel )
{
    oscc_error_t ret = OSCC_ERROR;

    canHandle handle = canOpenChannel( channel, canOPEN_EXCLUSIVE );

    if ( handle >= 0 )
    {
        canBusOff( handle );

        canStatus status = canSetBusParams( handle, BAUD_500K,
                                            0, 0, 0, 0, 0 );
        if ( status == canOK )
        {
            status = canSetBusOutputControl( handle, canDRIVER_NORMAL );

            if ( status == canOK )
            {
                status = canBusOn( handle );

                if ( status == canOK )
                {
                    oscc_data.can_handle = handle;
                    oscc_data.can_channel = channel;

                    status = canSetNotify(oscc_data.can_handle, oscc_update_status, canNOTIFY_RX, (char*)0);

                    if( status == canOK )
                    {
                        ret = OSCC_OK;
                    }
                    else
                    {
                        printf( "canSetNotify failed\n" );
                    }
                }
                else
                {
                    printf( "canBusOn failed\n" );
                }
            }
            else
            {
                printf( "canSetBusOutputControl failed\n" );
            }
        }
        else
        {
            printf( "canSetBusParams failed\n" );
        }
    }
    else
    {
        printf( "canOpenChannel %d failed\n", channel );
    }

    return ret;
}
