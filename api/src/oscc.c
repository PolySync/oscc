#include <stdio.h>
#include <stdlib.h>
#include <canlib.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "vehicles/vehicles.h"
#include "dtc.h"
#include "oscc.h"


static CanHandle can_handle;
static oscc_brake_command_s brake_cmd;
static oscc_throttle_command_s throttle_cmd;
static oscc_steering_command_s steering_cmd;

static void( *steering_report_callback )( oscc_steering_report_s *report );
static void( *brake_report_callback )( oscc_brake_report_s *report );
static void( *throttle_report_callback )( oscc_throttle_report_s *report );
static void( *fault_report_callback )( oscc_fault_report_s *report );
static void( *obd_frame_callback )( long id, unsigned char * data );

static oscc_error_t oscc_init_can( int channel );
static oscc_error_t oscc_can_write( long id, void* msg, unsigned int dlc );
static oscc_error_t oscc_enable_brakes( );
static oscc_error_t oscc_enable_throttle( );
static oscc_error_t oscc_enable_steering( );
static oscc_error_t oscc_disable_brakes( );
static oscc_error_t oscc_disable_throttle( );
static oscc_error_t oscc_disable_steering( );
static void oscc_update_status( canNotifyData *data );


oscc_error_t oscc_open( unsigned int channel )
{
    oscc_error_t ret = OSCC_ERROR;

    ret = oscc_init_can( channel );

    return ret;
}


oscc_error_t oscc_close( unsigned int channel )
{
    oscc_error_t ret = OSCC_ERROR;

    canStatus status = canWriteSync( can_handle, 1000 );

    if ( status == canOK )
    {
        status = canClose( can_handle );
    }

    if ( status == canOK )
    {
        ret = OSCC_OK;
    }

    return ret;
}


oscc_error_t oscc_enable( )
{
    oscc_error_t ret = oscc_enable_brakes( );

    if ( ret == OSCC_OK )
    {
        ret = oscc_enable_throttle( );

        if ( ret == OSCC_OK )
        {
            ret = oscc_enable_steering( );
        }
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

    brake_cmd.pedal_command = ( uint16_t ) BRAKE_POSITION_TO_PEDAL( brake_position );

    ret = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                    (void *) &brake_cmd,
                                    sizeof( brake_cmd ) );

    return ret;
}


oscc_error_t oscc_publish_brake_pressure( double brake_pressure )
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.pedal_command = ( uint16_t ) BRAKE_PRESSURE_TO_PEDAL( brake_pressure );

    ret = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                    (void *) &brake_cmd,
                                    sizeof( brake_cmd ) );

    return ret;
}


oscc_error_t oscc_publish_throttle_position( unsigned int throttle_position )
{
    oscc_error_t ret = OSCC_ERROR;

    throttle_cmd.spoof_value_low = ( uint16_t) THROTTLE_POSITION_TO_SPOOF_LOW( throttle_position );
    throttle_cmd.spoof_value_high = ( uint16_t ) THROTTLE_POSITION_TO_SPOOF_HIGH( throttle_position );

    ret = oscc_can_write( OSCC_THROTTLE_COMMAND_CAN_ID,
                                    (void *) &throttle_cmd,
                                    sizeof( throttle_cmd ) );

    return ret;
}


oscc_error_t oscc_publish_steering_angle( double angle )
{
    oscc_error_t ret = OSCC_ERROR;

    steering_cmd.spoof_value_low = ( int16_t ) STEERING_ANGLE_TO_SPOOF_LOW( angle );
    steering_cmd.spoof_value_high = ( int16_t ) STEERING_ANGLE_TO_SPOOF_HIGH( angle );

    ret = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                    (void *) &steering_cmd,
                                    sizeof( steering_cmd ) );

    return ret;
}


oscc_error_t oscc_publish_steering_torque( double torque )
{
    oscc_error_t ret = OSCC_ERROR;

    // MATHHHHHHHHHHH

    steering_cmd.spoof_value_low = ( int16_t ) STEERING_TORQUE_TO_SPOOF_LOW( torque );
    steering_cmd.spoof_value_high = ( int16_t ) STEERING_TORQUE_TO_SPOOF_HIGH( torque );

    ret = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                    (void *) &steering_cmd,
                                    sizeof( steering_cmd ) );

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


oscc_error_t oscc_subscribe_to_fault_reports( void( *callback )( oscc_fault_report_s *report ) )
{
    oscc_error_t ret = OSCC_ERROR;

    if ( callback != NULL )
    {
        fault_report_callback = callback;
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


static oscc_error_t oscc_enable_brakes( )
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.enable = 1;

    ret = oscc_publish_brake_position( 0 );

    return ret;
}


static oscc_error_t oscc_enable_throttle( )
{
    oscc_error_t ret = OSCC_ERROR;

    throttle_cmd.enable = 1;

    ret = oscc_publish_throttle_position( 0 );

    return ret;
}


static oscc_error_t oscc_enable_steering( )
{
    oscc_error_t ret = OSCC_ERROR;

    steering_cmd.enable = 1;

    ret = oscc_publish_steering_angle( 0 );

    return ret;
}


static oscc_error_t oscc_disable_brakes( )
{
    oscc_error_t ret = OSCC_ERROR;

    brake_cmd.enable = 0;

    ret = oscc_publish_brake_position( 0 );

    return ret;
}


static oscc_error_t oscc_disable_throttle( )
{
    oscc_error_t ret = OSCC_ERROR;

    throttle_cmd.enable = 0;

    ret = oscc_publish_throttle_position( 0 );

    return ret;
}


static oscc_error_t oscc_disable_steering( )
{
    oscc_error_t ret = OSCC_ERROR;

    steering_cmd.enable = 0;

    ret = oscc_publish_steering_angle( 0 );

    return ret;
}


static void oscc_update_status( canNotifyData *data )
{
    long can_id;
    unsigned int msg_dlc;
    unsigned int msg_flag;
    unsigned long tstamp;
    unsigned char buffer[ 8 ];

    canStatus can_status = canRead(
        can_handle,
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
        else if ( can_id == OSCC_FAULT_REPORT_CAN_ID ) {
            oscc_fault_report_s* fault_report =
                ( oscc_fault_report_s* )buffer;

            if (fault_report_callback != NULL)
            {
                fault_report_callback(fault_report);
            }
        }
        else
        {
            if ( obd_frame_callback != NULL )
            {
                obd_frame_callback( can_id, buffer );
            }
        }

        can_status = canRead(
            can_handle,
            &can_id,
            buffer,
            &msg_dlc,
            &msg_flag,
            &tstamp );
    }
}


static oscc_error_t oscc_can_write( long id, void* msg, unsigned int dlc )
{
    oscc_error_t ret = OSCC_ERROR;

    canStatus status = canWrite( can_handle, id, msg, dlc, 0 );

    if ( status == canOK )
    {
        ret = OSCC_OK;
    }

    return ret;
}


static oscc_error_t oscc_init_can( int channel )
{
    int ret = OSCC_OK;

    can_handle = canOpenChannel( channel, canOPEN_EXCLUSIVE );

    if ( can_handle < 0 )
    {
        printf( "canOpenChannel %d failed\n", channel );

        ret = OSCC_ERROR;
    }

    canStatus status;

    if ( ret != OSCC_ERROR )
    {
        status = canBusOff( can_handle );

        if ( status != canOK )
        {
            printf( "canBusOff failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if ( ret != OSCC_ERROR )
    {
        status = canSetBusParams( can_handle, BAUD_500K,
                                  0, 0, 0, 0, 0 );

        if ( status != canOK )
        {
            printf( "canSetBusParams failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if ( ret != OSCC_ERROR )
    {
        status = canSetBusOutputControl( can_handle, canDRIVER_NORMAL );

        if ( status != canOK )
        {
            printf( "canSetBusOutputControl failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if( ret != OSCC_ERROR )
    {
        status = canBusOn( can_handle );

        if ( status != canOK )
        {
            printf( "canBusOn failed\n" );

            ret = OSCC_ERROR;
        }
    }

    if( ret != OSCC_ERROR )
    {
        // register callback handler
        status = canSetNotify(

            can_handle,
            oscc_update_status,
            canNOTIFY_RX,
            (char*)0 );

        if ( status != canOK )
        {
            printf( "canSetNotify failed\n" );

            ret = OSCC_ERROR;
        }
    }

    return ret;
}
