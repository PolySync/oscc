/**
 * @file display.h
 * @brief Display functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_DISPLAY_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_DISPLAY_H_


/**
 * @brief Enumeration of possible screens.
 *
 */
typedef enum
{
    STATUS_SCREEN,
    ERROR_SCREEN,
    SCREEN_COUNT
} screen_t;


/**
 * @brief Enumeration of possible status pages.
 *
 */
typedef enum
{
    STATUS_PAGE_MAIN,
    STATUS_PAGE_COUNT
} status_page_t;


/**
 * @brief Enumeration of possible error pages.
 *
 */
typedef enum
{
    ERROR_PAGE_MAIN,
    ERROR_PAGE_COUNT
} error_page_t;


/**
 * @brief Enumeration of possible gateway statuses.
 *
 */
typedef enum
{
    GATEWAY_STATUS_UNKNOWN,
    GATEWAY_STATUS_GOOD,
    GATEWAY_STATUS_WARNING,
    GATEWAY_STATUS_ERROR
} gateway_status_t;


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
    status_page_t current_page;
    gateway_status_t gateway_status;
    module_status_t brake_status;
    module_status_t steering_status;
    module_status_t throttle_status;
} status_screen_s;


/**
 * @brief Current error screen state.
 *
 */
typedef struct
{
    error_page_t current_page;
} error_screen_s;


/**
 * @brief Current display state.
 *
 */
typedef struct
{
    screen_t current_screen;
    status_screen_s status_screen;
    error_screen_s error_screen;
} kia_soul_gateway_display_state_s;


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
