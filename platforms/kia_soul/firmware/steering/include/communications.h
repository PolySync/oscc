/**
 * @file communications.h
 * @brief Communication functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_


/*
 * @brief Scalar value to convert angle reported by OBD to human-readable value.
 *
 */
#define RAW_ANGLE_SCALAR ( 0.0076294 )

/*
 * @brief Scalar value to convert wheel angle (-40 to 40 degrees) to steering
 *        wheel angle (-470 to 470) degrees.
 *
 */
#define WHEEL_ANGLE_TO_STEERING_WHEEL_ANGLE_SCALAR ( 11.7 )


/*
 * @brief Amount of time after controller command that is considered a
 *        timeout. [milliseconds]
 *
 */
#define COMMAND_TIMEOUT_IN_MSEC ( 250 )


// ****************************************************************************
// Function:    publish_reports
//
// Purpose:     Publish all valid reports to CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void publish_reports( void );


// ****************************************************************************
// Function:    check_for_controller_command_timeout
//
// Purpose:     Check if the last command received from the controller exceeds
//              the timeout and disable control if it does.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_controller_command_timeout( void );


// ****************************************************************************
// Function:    check_for_incoming_message
//
// Purpose:     Check CAN bus for incoming messages and process any present.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void check_for_incoming_message( void );


#endif /* _OSCC_KIA_SOUL_STEERING_COMMUNICATIONS_H_ */
