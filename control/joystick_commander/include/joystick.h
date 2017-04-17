/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */

/* Permission is hereby granted, free of charge, to any person */
/* obtaining a copy of this software and associated documentation */
/* files (the “Software”), to deal in the Software without */
/* restriction, including without limitation the rights to use, */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following */
/* conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR */
/* OTHER DEALINGS IN THE SOFTWARE. */
/************************************************************************/

/**
 * @file joystick.h
 * @brief Joystick Interface.
 *
 */


#ifndef JOYSTICK_H
#define JOYSTICK_H



/**
 * @brief Lowest joystick axis value.
 *
 */
#define JOYSTICK_AXIS_POSITION_MIN ( -32768 )


/**
 * @brief Highest joystick axis value.
 *
 */
#define JOYSTICK_AXIS_POSITION_MAX ( 32767 )


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


/**
 * @brief Map axis value from one range to another.
 *
 * @param [in] position Input value to map.
 * @param [in] range_min Output minimum range.
 * @param [in] range_max Output maximum range.
 *
 * @return joystick position mapped to the range of min:max.
 *
 */
double joystick_normalize_axis_position( const int position,
                                         const double range_min,
                                         const double range_max );


/**
 * @brief Map trigger value from one range to another.
 *
 * @param [in] position Input value to map.
 * @param [in] range_min Output minimum range.
 * @param [in] range_max Output maximum range.
 *
 * @return position mapped to the range of min:max.
 *
 */
double joystick_normalize_trigger_position( const int position,
                                            const double range_min,
                                            const double range_max );


#endif	/* JOYSTICK_H */
