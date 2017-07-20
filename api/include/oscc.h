/**
 * @file oscc.h
 * @brief OSCC interface - Register callbacks for retrieving module and vehicle reports,
 *        and send requested targets to the modules.
 */


#ifndef _OSCC_H
#define _OSCC_H

#include <stdbool.h>
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"


typedef enum
{
    OSCC_OK,
    OSCC_ERROR,
    OSCC_WARNING
} oscc_error_t;


/**
 * @brief Use provided CAN channel to open communications
 *        to CAN bus connected to the OSCC modules.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_open( unsigned int channel );


/**
 * @brief Use provided CAN channel to close communications
 *        to CAN bus connected to the OSCC modules.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_close( unsigned int channel );


/**
 * @brief Send enable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_enable( );


/**
 * @brief Send disable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_disable( );


/**
 * @brief Publish message with requested brake pedal position to
 *        brake module.
 *
 * @param [in] position - Requested brake pedal position.
 *              @toDO - decide on units/range (mm?)
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_publish_brake_position( unsigned int brake_position );


/**
 * @brief Publish message with requested brake pressure to
 *        brake module.
 *
 * @param [in] pressure - Requested brake pressure.
 *              @toDo - decide on units / range
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_publish_brake_pressure( double brake_pressure );


/**
 * @brief Publish message with requested throttle pedal position to
 *        throttle module.
 *
 * @param [in] position - Requested throttle pedal position.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_publish_throttle_position( unsigned int throttle_position );


/**
 * @brief Publish message with requested steering angle to
 *        steering module.
 *
 * @param [in] angle - Requested steering angle (degrees).
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_publish_steering_angle( double angle );


/**
 * @brief Publish message with requested steering torque to
 *        steering module.
 *
 * @param [in] angle - Requested steering torque (Newton-meters).
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_publish_steering_torque( double torque );


/**
 * @brief Register callback function to be called when brake report
 *        received from brake module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *                        brake report received from brake module.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_subscribe_to_brake_reports( void( *callback )( oscc_brake_report_s *report ) );


/**
 * @brief Register callback function to be called when throttle report
 *        received from throttle module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *                        throttle report received from throttle module.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_subscribe_to_throttle_reports( void( *callback )( oscc_throttle_report_s *report ) );


/**
 * @brief Register callback function to be called when steering report
 *        received from steering module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *                        steering report received from steering module.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_subscribe_to_steering_reports( void( *callback )( oscc_steering_report_s *report ) );


/**
 * @brief Register callback function to be called when fault report
 *        received from any module.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *                        fault report received from any module.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_subscribe_to_fault_reports( void( *callback )( oscc_fault_report_s *report ) );


/**
 * @brief Register callback function to be called when OBD message received
 *        from vehicle.
 *
 * @param [in] callback - Pointer to callback function to be called when
 *                        OBD message received.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_error_t oscc_subscribe_to_obd_messages( void( *callback )( long id, unsigned char * data ) );


#endif /* _OSCC_H */

