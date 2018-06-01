/**
 * @file oscc.h
 * @brief OSCC interface - Register callbacks for retrieving module and vehicle reports,
 *        and send requested targets to the modules.
 */


#ifndef _OSCC_H
#define _OSCC_H


#include <linux/can.h>

#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"
#include "vehicles.h"


/*
 * @brief MAX_CAN_IDS is the maximum number unique CAN IDs on the CAN bus used
 * for auto detection of CAN channels. Increasing this number increases the wait
 * time for checking if a channel contains expected CAN IDs, reducing this
 * number below number of CAN IDs broadcast could yield a false negative in auto
 * detection.
 *
 */
#define MAX_CAN_IDS ( 70 )


/*
 * @brief CAN_MESSAGE_TIMEOUT is the time to wait for a CAN message in
 * milliseconds used for auto detection of can channels.
 *
 */
#define CAN_MESSAGE_TIMEOUT ( 100 )


typedef enum
{
    OSCC_OK,
    OSCC_ERROR,
    OSCC_WARNING
} oscc_result_t;

/**
 * @brief Looks for available CAN channels and automatically detects which
 *        channel is OSCC control and which channel is vehicle CAN for feedback.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_init();

/**
 * @brief Use provided CAN channel to open communications to CAN bus connected
 *        to the OSCC modules. If CAN gateway does not forward Vehicle CAN
 *        automatically detect if a CAN channel has Vehicle CAN available.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_open( unsigned int channel );


/**
 * @brief Use provided CAN channel to close communications
 *        to CAN bus connected to the OSCC modules.
 *
 * @param [in] channel - CAN channel connected to OSCC modules.
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_close( unsigned int channel );


/**
 * @brief Send enable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_enable( void );


/**
 * @brief Send disable commands to all OSCC modules.
 *
 * @param [void]
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_disable( void );


/**
 * @brief Publish message with requested brake pedal position to
 *        brake module.
 *
 * @param [in] position - Normalized requested brake pedal
 *        position in the range [0, 1].
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_publish_brake_position( double brake_position );


/**
 * @brief Publish message with requested throttle pedal position to
 *        throttle module.
 *
 * @param [in] position - Normalized requested throttle pedal
 *        position in the range [0, 1].
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_publish_throttle_position( double throttle_position );


/**
 * @brief Publish message with requested steering torque to
 *        steering module.
 *
 * @param [in] angle - Normalized requested steering wheel
 *        torque in the range [-1, 1].
 *
 * @return OSCC_ERROR or OSCC_OK
 *
 */
oscc_result_t oscc_publish_steering_torque( double torque );


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
oscc_result_t oscc_subscribe_to_brake_reports( void( *callback )( oscc_brake_report_s *report ) );


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
oscc_result_t oscc_subscribe_to_throttle_reports( void( *callback )( oscc_throttle_report_s *report ) );


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
oscc_result_t oscc_subscribe_to_steering_reports( void( *callback )( oscc_steering_report_s *report ) );


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
oscc_result_t oscc_subscribe_to_fault_reports( void( *callback )( oscc_fault_report_s *report ) );


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
oscc_result_t oscc_subscribe_to_obd_messages( void( *callback )( struct can_frame *frame ) );


/**
 * @brief Set vehicle right rear wheel speed in kph from CAN frame. (kph)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with wheel speed (CAN ID: \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
 *
 * @param [out] wheel_speed_right_rear - A pointer to double. Set to the unpacked and scaled rear
 * right wheel speed reported by the vehicle (kph).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID
 */
oscc_result_t get_wheel_speed_right_rear(
    struct can_frame const * const frame,
    double * wheel_speed_right_rear);

/**
 * @brief Get vehicle left rear wheel speed in kph from CAN frame. (kph)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with wheel speed (CAN ID: \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
 *
 * @param [out] wheel_speed_left_rear - A pointer to double. Set to the unpacked and scaled front
 * left wheel speed reported by the vehicle (kph).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID
 */
oscc_result_t get_wheel_speed_left_rear(
    struct can_frame const * const frame,
    double * wheel_speed_left_rear);


/**
 * @brief Get vehicle right front wheel speed in kph from CAN frame. (kph)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with wheel speed (CAN ID: \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
 *
 * @param [out] wheel_speed_right_front - A pointer to double. Set to the unpacked and scaled front
 * right wheel speed reported by the vehicle (kph).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID
 */
oscc_result_t get_wheel_speed_right_front(
    struct can_frame const * const frame,
    double * wheel_speed_right_front);


/**
 * @brief Get vehicle left front wheel speed in kph from CAN frame. (kph)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with wheel speed (CAN ID: \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID)
 *
 * @param [out] wheel_speed_left_front - A pointer to double. Set to the unpacked and scaled rear
 * left wheel speed reported by the vehicle (kph).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_WHEEL_SPEED_CAN_ID
 */
oscc_result_t get_wheel_speed_left_front(
    struct can_frame const * const frame,
    double * wheel_speed_left_front);


/**
 * @brief Get vehicle steering wheel angle from CAN frame. (degrees)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with steering wheel angle (CAN ID: \ref KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID)
 *
 * @param [out] steering_wheel_angle - A pointer to double. Value is set to the unpacked and scaled
 * steering wheel angle reported by the vehicle (degrees).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_STEERING_WHEEL_ANGLE_CAN_ID
 */
oscc_result_t get_steering_wheel_angle(
    struct can_frame const * const frame,
    double * steering_wheel_angle);


/**
 * @brief Get vehicle brake pressure from CAN frame. (bar)
 *
 * @param [in] frame - A pointer to \ref struct can_frame that contains the raw CAN data associated
 * with brake pressure (CAN ID: \ref KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID)
 *
 * @param [out] brake_pressure - A pointer to double. Set to the unpacked and scaled brake pressure
 * reported by the vehicle (bar).
 *
 * @return:
 * \li \ref OSCC_OK on successful unpacking.
 * \li \ref OSCC_ERROR if a parameter is NULL or the CAN frame ID is not
 * \ref KIA_SOUL_OBD_BRAKE_PRESSURE_CAN_ID
 */
oscc_result_t get_brake_pressure(
    struct can_frame const * const frame,
    double * brake_pressure);


#endif /* _OSCC_H */
