/**
 * @file joystick.h
 * @brief Joystick Interface.
 *
 */


#ifndef JOYSTICK_H
#define JOYSTICK_H

/**
 * @brief Button state not pressed.
 *
 */
#define JOYSTICK_BUTTON_STATE_NOT_PRESSED ( 0 )


/**
 * @brief Button state pressed.
 *
 */
#define JOYSTICK_BUTTON_STATE_PRESSED ( 1 )


/**
 * @brief Initialization function for the joystick
 *
 * @return ERROR code
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int joystick_init( );


/**
 * @brief Open joystick device.
 *
 * @param [in] device_index Device index in the subsystem.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int joystick_open( unsigned long device_index );


/**
 * @brief Close joystick device.
 *
 * @return void
 *
 */
void joystick_close( );


/**
 * @brief Update joystick device.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int joystick_update( );


/**
 * @brief Get joystick axis value.
 *
 * @param [in] axis_index Axis index.
 * @param [out] position Current axis value.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int joystick_get_axis( const unsigned long axis_index, int * const position );


/**
 * @brief Get joystick button state.
 *
 * @param [in] button_index Button index.
 * @param [out] state Current button state.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int joystick_get_button( const unsigned long button_index,
                         unsigned int * const state );

#endif /* JOYSTICK_H */