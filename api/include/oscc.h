/**
 * @file oscc.h
 * @brief OSCC interface - Register callbacks for retrieving module and vehicle reports,
 *        and send requested targets to the modules.
 */


#ifndef OSCC_H
#define OSCC_H

#include <stdbool.h>
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"

typedef struct
{
    float wheel_speed_front_left;
    float wheel_speed_front_right;
    float wheel_speed_rear_left;
    float wheel_speed_rear_right;
} oscc_wheel_speed_s;


// NEED TO REVISIT THIS POSSIBLY -- How do we want to pass these messages?

typedef struct
{
    float current_steering_wheel_angle;
    float current_vehicle_brake_pressure;
    oscc_wheel_speed_s current_vehicle_wheel_speeds;
} oscc_obd_message_s;

typedef struct
{
    bool operator_override;
    bool fault_brake_obd_timeout;
    bool fault_brake_invalid_sensor_value;
    bool fault_brake_actuator_error;
    bool fault_brake_pump_motor_error;
    bool fault_steering_obd_timeout;
    bool fault_steering_invalid_sensor_value;
    bool fault_throttle_invalid_sensor_value;
} oscc_status_s;


/**
 * @brief Use provided CAN channel to open communications
 *        to CAN bus connected to the OSCC modules.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_open( unsigned int channel );


/**
 * @brief Use provided CAN channel to close communications
 *        to CAN bus connected to the OSCC modules.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return ERROR or NOERR
 *
 */
void oscc_close( unsigned int channel );


/**
 * @brief Send enable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_enable( );


/**
 * @brief Send disable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_disable( );


/**
 * @brief Publish message with requested brake pedal position to
 *        brake module.
 *
 * @param [in] position - Requested brake pedal position.
 *              @toDO - decide on units/range (mm?)
 *
 * @return ERROR or NOERR
 *
 */
int oscc_publish_brake_position( unsigned int brake_position );


/**
 * @brief Publish message with requested brake pressure to
 *        brake module.
 *
 * @param [in] pressure - Requested brake pressure.
 *              @toDo - decide on units / range
 *
 * @return ERROR or NOERR
 *
 */
int oscc_publish_brake_pressure( double brake_pressure );


/**
 * @brief Publish message with requested throttle pedal position to
 *        throttle module.
 *
 * @param [in] position - Requested throttle pedal position.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_publish_throttle_position( unsigned int throttle_position );


/**
 * @brief Publish message with requested steering angle to
 *        steering module.
 *
 * @param [in] angle - Requested steering angle (degrees).
 *
 * @return ERROR or NOERR
 *
 */
int oscc_publish_steering_angle( double angle );


/**
 * @brief Publish message with requested steering torque to
 *        steering module.
 *
 * @param [in] angle - Requested steering torque (Newton-meters).
 *
 * @return ERROR or NOERR
 *
 */
int oscc_publish_steering_torque( double torque );


/**
 * @brief Register callback function to be called when brake report
 *        recieved from brake module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *      .                 brake report recieved from brake module.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_subscribe_to_brake_reports( void( *callback )( oscc_brake_report_s *report ) );


/**
 * @brief Register callback function to be called when throttle report
 *        recieved from throttle module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *      .                 throttle report recieved from throttle module.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_subscribe_to_throttle_reports( void( *callback )( oscc_throttle_report_s *report ) );


/**
 * @brief Register callback function to be called when steering report
 *        recieved from steering module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *      .                 steering report recieved from steering module.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_subscribe_to_steering_reports( void( *callback )( oscc_steering_report_s *report ) );


/**
 * @brief Register callback function to be called when OBD message recieved
 *        from veihcle.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *      .                 OBD message recieved.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_subscribe_to_obd_messages( void( *callback )( oscc_obd_message_s *message ) );


#endif /* OSCC_H */

