/**
 * @file oscc.c
 * @brief OSCC interface source- The main command* functions and
 *        the update function should be called on at least a
 *        50ms period.  The expectation is that if there is not
 *        some kind of communication from the controller to the
 *        OSCC modules in that time, then the OSCC modules will
 *        disable and return control back to the driver.
 */


#include <stdio.h>
#include <stdlib.h>
#include <canlib.h>

#include "macros.h"
#include "dtc.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "oscc.h"


// *****************************************************
// static global types/macros
// *****************************************************

/**
 * @brief OSCC command data - container for the various CAN
 *        messages that are used to control the brakes, steering
 *        and throttle.  In addition, there are additional
 *        variables to store the CAN parameters, handle and
 *        channel.
 *
 *        The entire structure is packed at the single byte
 *        level because of the need to send it on the wire to
 *        a receiver that is expecting a specific layout.
 */

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


// *****************************************************
// static global data
// *****************************************************

static oscc_command_data_s oscc_data;
static oscc_command_data_s* oscc = NULL;


// *****************************************************
// static definitions
// *****************************************************

// *****************************************************
// Function:    oscc_check_for_operator_override
//
// Purpose:     Checks report messages for override flag.
//
// Returns:     bool - override occurred flag
//
// Parameters:  can_id - ID of CAN frame containing the report
//              buffer - Buffer of CAN frame containing the report
//
// *****************************************************
static bool oscc_check_for_operator_override(
    long can_id,
    unsigned char * buffer )
{
    if ( can_id == OSCC_BRAKE_REPORT_CAN_ID )
    {
        oscc_brake_report_s* brake_report =
            ( oscc_brake_report_s* )buffer;

        // status->operator_override = (bool) brake_report->override;
    }
    else if ( can_id == OSCC_THROTTLE_REPORT_CAN_ID )
    {
        oscc_throttle_report_s* throttle_report =
            ( oscc_throttle_report_s* )buffer;

        // status->operator_override = (bool) throttle_report->operator_override;
    }
    else if ( can_id == OSCC_STEERING_REPORT_CAN_ID )
    {
        oscc_steering_report_s* steering_report =
            ( oscc_steering_report_s* )buffer;

        // status->operator_override = (bool) steering_report->operator_override;
    }
}

// *****************************************************
// Function:    oscc_check_for_obd_timeouts
//
// Purpose:     Checks report messages for OBD timeout flag.
//
// Returns:     bool - timeout occurred flag
//
// Parameters:  can_id - ID of CAN frame containing the report
//              buffer - Buffer of CAN frame containing the report
//
// *****************************************************
static void oscc_check_for_obd_timeout(
    long can_id,
    unsigned char * buffer )
{
    // no longer detecting these at the module level

    // if ( can_id == OSCC_BRAKE_REPORT_CAN_ID )
    // {
    //     oscc_brake_report_s* brake_report =
    //         ( oscc_brake_report_s* )buffer;

    //     status->obd_timeout_brake = (bool) brake_report->fault_obd_timeout;
    // }
    // else if ( can_id == OSCC_STEERING_REPORT_CAN_ID )
    // {
    //     oscc_steering_report_s* steering_report =
    //         ( oscc_steering_report_s* )buffer;

    //     status->obd_timeout_steering = (bool) steering_report->fault_obd_timeout;
    // }
}

// **********************************************************
// Function:    oscc_check_for_invalid_sensor_value
//
// Purpose:     Checks report messages for invalid sensor value flag.
//
// Returns:     bool - invalid sensor value flag
//
// Parameters:  can_id - ID of CAN frame containing the report
//              buffer - Buffer of CAN frame containing the report
//
// **********************************************************
static void oscc_check_for_invalid_sensor_value(
    long can_id,
    unsigned char * buffer )
{
    // if ( can_id == OSCC_BRAKE_REPORT_CAN_ID )
    // {
    //     oscc_brake_report_s* brake_report =
    //         ( oscc_brake_report_s* )buffer;

    //     status->invalid_sensor_value_brake = brake_report->fault_invalid_sensor_value;
    // }
    // else if ( can_id == OSCC_STEERING_REPORT_CAN_ID )
    // {
    //     oscc_steering_report_s* steering_report =
    //         ( oscc_steering_report_s* )buffer;

    //     status->invalid_sensor_value_steering = DTC_CHECK(steering_report->dtcs, OSCC_STEERING_DTC_INVALID_SENSOR_VAL);
    // }
    // else if ( can_id == OSCC_THROTTLE_REPORT_CAN_ID )
    // {
    //     oscc_throttle_report_s* throttle_report =
    //         ( oscc_throttle_report_s* )buffer;

    //     status->invalid_sensor_value_throttle = DTC_CHECK(throttle_report->dtcs, OSCC_THROTTLE_DTC_INVALID_SENSOR_VAL);
    // }
}

// *****************************************************
// Function:    oscc_disable_brakes
//
// Purpose:     Send a specific CAN message to set the brake enable value
//              to 0.  Included with this is a safe brake setting
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int oscc_disable_brakes( )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.enabled = 0;

        printf( "brake: %d %d\n", oscc->brake_cmd.enabled,
                oscc->brake_cmd.pedal_command );

        return_code = oscc_publish_brake_position( 0 );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_disable_throttle
//
// Purpose:     Send a specific CAN message to set the throttle enable value
//              to 0.  Included with this is a safe throttle setting
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int oscc_disable_throttle( )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->throttle_cmd.enable = 0;

        printf( "throttle: %d %d %d\n", oscc->throttle_cmd.enable,
                oscc->throttle_cmd.spoof_value_low,
                oscc->throttle_cmd.spoof_value_high );

        return_code = oscc_publish_throttle_position( 0 );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_disable_steering
//
// Purpose:     Send a specific CAN message to set the steering enable value
//              to 0.  Included with this is a safe steering angle and rate
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
static int oscc_disable_steering( )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->steering_cmd.enable = 0;

        printf( "steering: %d %d %d\n",
                oscc->steering_cmd.enable,
                oscc->steering_cmd.spoof_value_low,
                oscc->steering_cmd.spoof_value_high );

        return_code = oscc_publish_steering_angle( 0 );
    }
    return ( return_code );
}


// make this shit a callback function for the canlib
// *****************************************************
// Function:    oscc_update_status
//
// Purpose:     Read CAN messages from the OSCC modules and check for  status
//              changes.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  override - pointer to an integer value that is filled out if
//              the OSCC modules indicate any override status
//
// *****************************************************
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

        if ( can_status == canOK )
        {
            printf("id: 0x%lx\n", can_id);

            // oscc_check_for_operator_override( can_id, buffer );

            // oscc_check_for_obd_timeout( can_id, buffer );

            // oscc_check_for_invalid_sensor_value( can_id, buffer );
        }
        else if( ( can_status == canERR_NOMSG ) || ( can_status == canERR_TIMEOUT ) )
        {
            // Do nothing
        }
        else
        {
        }
    }
}

// *****************************************************
// Function:    oscc_can_write
//
// Purpose:     Wrapper around the canWrite routine from the CAN library
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  id - ID of the CAN message ot send
//              msg - pointer to the buffer to send
//              dlc - size of the buffer
//
// *****************************************************
static int oscc_can_write( long id, void* msg, unsigned int dlc )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        canStatus status = canWrite( oscc->can_handle, id, msg, dlc, 0 );

        if ( status == canOK )
        {
            return_code = NOERR;
        }
    }
    return return_code;
}

// *****************************************************
// Function:    oscc_init_can
//
// Purpose:     Initialize the OSCC communication layer with known values
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  channel - for now, the CAN channel to use when interacting
//              with the OSCC modules
//
// *****************************************************
static int oscc_init_can( int channel )
{
    int return_code = ERROR;

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

                    // register callback handler
                    status = canSetNotify(oscc_data.can_handle, oscc_update_status, canNOTIFY_RX | canNOTIFY_TX | canNOTIFY_ERROR, (char*)0);

                    if( status == canOK )
                    {
                        return_code = NOERR;
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
    return return_code;
}


// *****************************************************
// public definitions
// *****************************************************

// *****************************************************
// Function:    oscc_open
//
// Purpose:     Initialize the OSCC interface - CAN communication
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  channel - integer value containing the CAN channel to open
//
// *****************************************************
int oscc_open( unsigned int channel )
{
    int return_code = ERROR;

    return_code = oscc_init_can( channel );

    if ( return_code == NOERR )
    {
        oscc = &oscc_data;
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_close
//
// Purpose:     Release resources and close the interface
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  channel - integer value containing the CAN channel to close
//
// *****************************************************
void oscc_close( unsigned int channel )
{
    if ( oscc != NULL )
    {
        canWriteSync( oscc->can_handle, 1000 );
        canClose( oscc->can_handle );
    }

    oscc = NULL;
}

// *****************************************************
// Function:    oscc_enable
//
// Purpose:     Cause the initialized interface to enable control of the
//              vehicle using the OSCC modules
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
int oscc_enable( )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        return_code = NOERR;

        oscc->brake_cmd.enabled = 1;
        oscc->throttle_cmd.enable = 1;
        oscc->steering_cmd.enable = 1;
    }

    return ( return_code );
}

// *****************************************************
// Function:    oscc_disable
//
// Purpose:     Send a series of CAN messages to disable all of the OSCC
//              modules.  Mostly a wrapper around the existing specific
//              disable functions
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  void
//
// *****************************************************
int oscc_disable( )
{
    int return_code = oscc_disable_brakes( );

    if ( return_code == NOERR )
    {
        return_code = oscc_disable_throttle( );

        if ( return_code == NOERR )
        {
            return_code = oscc_disable_steering( );
        }
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_publish_brake_position
//
// Purpose:     Send a CAN message to set the brakes to a requested position.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  brake_position - unsigned value
//              The value is range limited between 0 and 52428
//
// *****************************************************
int oscc_publish_brake_position( unsigned int brake_position )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.pedal_command = ( uint16_t )brake_position;

        return_code = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                      (void *) &oscc->brake_cmd,
                                      sizeof( oscc->brake_cmd ) );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_publish_brake_pressure
//
// Purpose:     Send a CAN message to set the brakes to a requested pressure.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  brake_pressure - double
//              The value is range limited between 0 and 52428
//
// *****************************************************
int oscc_publish_brake_pressure( double brake_pressure )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->brake_cmd.pedal_command = ( uint16_t )brake_pressure;

        return_code = oscc_can_write( OSCC_BRAKE_COMMAND_CAN_ID,
                                      (void *) &oscc->brake_cmd,
                                      sizeof( oscc->brake_cmd ) );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_publish_throttle_position
//
// Purpose:     Send a CAN message to set the throttle to a requested position.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  throttle_position - unsigned value
//              The value is range limited between 0 and 19660
//
// *****************************************************
int oscc_publish_throttle_position( unsigned int throttle_position )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        // oscc->throttle_cmd.commanded_accelerator_position = ( uint16_t )throttle_setpoint;

        // MATHHHHHHHHHHH

        oscc->throttle_cmd.spoof_value_low = ( uint16_t )throttle_position;
        oscc->throttle_cmd.spoof_value_high = ( uint16_t )throttle_position;

        return_code = oscc_can_write( OSCC_THROTTLE_COMMAND_CAN_ID,
                                      (void *) &oscc->throttle_cmd,
                                      sizeof( oscc->throttle_cmd ) );
    }

    return ( return_code );
}


// *****************************************************
// Function:    oscc_publish_steering_angle
//
// Purpose:     Send a CAN message to set the steering to a requested angle.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  angle - double: the steering angle in degrees
//
// *****************************************************
int oscc_publish_steering_angle( double angle )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->steering_cmd.spoof_value_low = ( int16_t )angle;
        oscc->steering_cmd.spoof_value_high = ( int16_t )angle;

        return_code = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                      (void *) &oscc->steering_cmd,
                                      sizeof( oscc->steering_cmd ) );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_publish_steering_torque
//
// Purpose:     Send a CAN message to apply a requested torque to steering wheel.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  torque - double: the requested torque in Nm.
//
// *****************************************************
int oscc_publish_steering_torque( double torque )
{
    int return_code = ERROR;

    if ( oscc != NULL )
    {
        oscc->steering_cmd.spoof_value_low = ( int16_t )torque;
        oscc->steering_cmd.spoof_value_high = ( int16_t )torque;

        return_code = oscc_can_write( OSCC_STEERING_COMMAND_CAN_ID,
                                      (void *) &oscc->steering_cmd,
                                      sizeof( oscc->steering_cmd ) );
    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_subscribe_to_brake_reports
//
// Purpose:     Register callback function to be called when brake reports are
//              recieved from brake module.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  torque - double: the requested torque in Nm.
//
// *****************************************************
int oscc_subscribe_to_brake_reports( void( *callback )( oscc_brake_report_s *report ) )
{
    int return_code = ERROR;

    // if callback is a thing, register it someplace
    if ( oscc != NULL )
    {

    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_subscribe_to_brake_reports
//
// Purpose:     Register callback function to be called when brake reports are
//              recieved from brake module.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  torque - double: the requested torque in Nm.
//
// *****************************************************
int oscc_subscribe_to_throttle_reports( void( *callback )( oscc_throttle_report_s *report ) )
{
    int return_code = ERROR;

    // if callback is a thing, register it someplace
    if ( oscc != NULL )
    {

    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_subscribe_to_brake_reports
//
// Purpose:     Register callback function to be called when brake reports are
//              recieved from brake module.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  torque - double: the requested torque in Nm.
//
// *****************************************************
int oscc_subscribe_to_steering_reports( void( *callback )( oscc_steering_report_s *report ) )
{
    int return_code = ERROR;

    // if callback is a thing, register it someplace
    if ( oscc != NULL )
    {

    }
    return ( return_code );
}

// *****************************************************
// Function:    oscc_subscribe_to_brake_reports
//
// Purpose:     Register callback function to be called when brake reports are
//              recieved from brake module.
//
// Returns:     int - ERROR or NOERR
//
// Parameters:  torque - double: the requested torque in Nm.
//
// *****************************************************
int oscc_subscribe_to_obd_messages( void( *callback )( oscc_obd_message_s *message ) )
{
    int return_code = ERROR;

    // if callback is a thing, register it someplace
    if ( oscc != NULL )
    {

    }
    return ( return_code );
}
