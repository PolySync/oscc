/**
 * @file communications.h
 * @brief Communication functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_


#include <stdint.h>

#include "globals.h"


/*
 * @brief Set warning flag in heartbeat warning register.
 *
 */
#define SET_HEARTBEAT_WARNING(x) (g_tx_heartbeat.data.warning_register |= ((uint16_t) x))

/*
 * @brief Clear warning flag in heartbeat warning register.
 *
 */
#define CLEAR_HEARTBEAT_WARNING(x) (g_tx_heartbeat.data.warning_register &= ~((uint16_t) x))

/*
 * @brief Set error flag in heartbeat error register.
 *
 */
#define SET_HEARTBEAT_ERROR(x) (g_tx_heartbeat.data.error_register |= ((uint16_t) x))

/*
 * @brief Clear error flag in heartbeat error register.
 *
 */
#define CLEAR_HEARTBEAT_ERROR(x) (g_tx_heartbeat.data.error_register &= ~((uint16_t) x))

/*
 * @brief Set heartbeat state.
 *
 */
#define SET_HEARTBEAT_STATE(x) (g_tx_heartbeat.data.state = ((uint8_t) x))

/*
 * @brief Get heartbeat state.
 *
 */
#define GET_HEARTBEAT_STATE() (g_tx_heartbeat.data.state)

/*
 * @brief Set Chassis 1 flag.
 *
 */

#define SET_CHASSIS_1_FLAG(x) (g_tx_chassis_state_1.data.flags |= ((uint8_t) x))

/*
 * @brief Clear Chassis 1 flag.
 *
 */
#define CLEAR_CHASSIS_1_FLAG(x) (g_tx_chassis_state_1.data.flags &= ~((uint8_t) x))


// ****************************************************************************
// Function:    publish_heartbeat_report
//
// Purpose:     Fill out heartbeat report and publish it to the CAN bus.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void publish_heartbeat_report( void );


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


#endif /* _OSCC_KIA_SOUL_CAN_GATEWAY_COMMUNICATIONS_H_ */
