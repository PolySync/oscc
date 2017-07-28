/**
 * @file commander.h
 * @brief Commander Interface.
 *
 */


#ifndef COMMANDER_H
#define COMMANDER_H


/**
 * @brief Initialize the commander for use
 *
 * @param [in] channel to use to communicate with OSCC modules
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int commander_init( int channel );

/**
 * @brief Close the commander.  Releases and closes all modules
 *        under the commander also.
 *
 * @param [in] channel used to communicate with OSCC modules
 *
 * @return void
 *
 */
void commander_close( int channel );

/**
 * @brief Checks the status of the joystick and the the OSCC modules 
 * and updates the current values, including brakes, throttle and 
 * steering. Is expected to execute every 50ms.
 *
 * @param [void]
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int check_for_controller_update( );


#endif /* COMMANDER_H */
