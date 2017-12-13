/**
 * @file display.h
 * @brief Display functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_DISPLAY_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_DISPLAY_H_


#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/steering_can_protocol.h"
#include "can_protocols/throttle_can_protocol.h"


/*
* @brief Frequency of updates of display content. [Hz]
*
*/
#define DISPLAY_UPDATE_FREQUENCY_IN_HZ ( 4 )


/**
 * @brief Enumeration of possible screens.
 *
 */
typedef enum
{
    STATUS_SCREEN,
    DTC_SCREEN,
    SCREEN_COUNT
} screen_t;


/**
 * @brief Enumeration of possible module statuses.
 *
 */
typedef enum
{
    MODULE_STATUS_UNKNOWN,
    MODULE_STATUS_ENABLED,
    MODULE_STATUS_DISABLED,
    MODULE_STATUS_ERROR,
} module_status_t;


/**
 * @brief Current status screen state.
 *
 */
typedef struct
{
    module_status_t brakes;
    module_status_t steering;
    module_status_t throttle;
} status_screen_s;


/**
 * @brief Current DTC screen state.
 *
 */
typedef struct
{
    bool brakes[OSCC_BRAKE_DTC_COUNT];
    bool steering[OSCC_STEERING_DTC_COUNT];
    bool throttle[OSCC_THROTTLE_DTC_COUNT];
} dtc_screen_s;


/**
 * @brief Current display state.
 *
 */
typedef struct
{
    screen_t current_screen;
    status_screen_s status_screen;
    dtc_screen_s dtc_screen;
} kia_soul_gateway_display_state_s;


// ****************************************************************************
// Function:    init_display
//
// Purpose:     Initializes the display.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_display( void );


// ****************************************************************************
// Function:    update_display
//
// Purpose:     Updates the display with new information.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void update_display( void );


#endif /* _OSCC_KIA_SOUL_CAN_GATEWAY_DISPLAY_H_ */
