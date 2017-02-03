/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2016 PolySync Technologies, Inc.  All Rights Reserved. */

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
 * @brief Invalid \ref joystick_device_s.handle value.
 *
 */
#define JOYSTICK_DEVICE_HANDLE_INVALID ( NULL )


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
 * @brief Joystick Identifier Data
 *
 */
#define JOYSTICK_ID_DATA_SIZE ( 16 )

/**
 * @brief Joystick Description String
 *
 */
#define JOYSTICK_ID_STRING_SIZE ( 64 )




/**
 * @brief Joystick GUID.
 *
 * Implementation-dependent GUID
 *
 */
typedef struct
{
    unsigned char data[ JOYSTICK_ID_DATA_SIZE ];
    char ascii_string[ JOYSTICK_ID_STRING_SIZE ];

} joystick_guid_s;


typedef struct
{
    //
    //
    double brake_setpoint_average;
    //
    //
    double throttle_setpoint_average;
    //
    //
    double steering_setpoint_average;
    //
    //
    double last_joystick_state_steering;
} joystick_state_s;


/**
 * @brief Joystick device.
 *
 */
typedef struct
{
    //
    //
    void *handle;
    //
    //
    joystick_guid_s guid;
    //
    //
    joystick_state_s joystick_state;

} joystick_device_s;




/**
 * @brief Initialize joystick state struct.
 *
 * @return ERROR code
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_init_state( joystick_device_s * const jstick );


/**
 * @brief Initialize joystick subsystem.
 *
 * @return ERROR code
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_init_subsystem( );


/**
 * @brief Release joystick subsystem.
 *
 * @return void
 *
 */
void jstick_release_subsystem( );


/**
 * @brief Get number of joystick devices.
 *
 * @return
 * \li -1 on error
 * \li >=0 device count
 *
 */
int jstick_get_num_devices( void );


/**
 * @brief Get joystick GUID.
 *
 * @param [in] device_index Device index in the subsystem.
 * @param [out] guid A pointer to \ref joystick_guid_s which receives the GUID value.
 *
 * @return ERROR code
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_get_guid_at_index( const unsigned long device_index,
                              joystick_guid_s * const guid );


/**
 * @brief Open joystick device.
 *
 * @param [in] device_index Device index in the subsystem.
 * @param [out] jstick A pointer to \ref joystick_device_s which receives the configuration.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_open( const unsigned long device_index,
                 joystick_device_s * const jstick );


/**
 * @brief Close joystick device.
 *
 * @param [out] jstick A pointer to \ref joystick_device_s which is to be closed.
 *
 * @return void
 *
 */
void jstick_close( joystick_device_s * const jstick );


/**
 * @brief Update joystick device.
 *
 * @param [out] jstick A pointer to \ref joystick_device_s which receives the update.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_update( joystick_device_s * const jstick );


/**
 * @brief Get joystick axis value.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the configuration.
 * @param [in] axis_index Axis index.
 * @param [out] position Current axis value.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_get_axis( joystick_device_s * const jstick,
                     const unsigned long axis_index,
                     int * const position );


/**
 * @brief Get joystick button state.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the configuration.
 * @param [in] button_index Button index.
 * @param [out] state Current button state.
 *
 * @return ERROR code:
 * \li \ref NOERR (1) if success.
 * \li \ref ERROR (0) if failure.
 *
 */
int jstick_get_button( joystick_device_s * const jstick,
                       const unsigned long button_index,
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
double jstick_normalize_axis_position( const int position,
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
double jstick_normalize_trigger_position(
        const int position,
        const double range_min,
        const double range_max );


/**
 * @brief Calculate exponential average from current and previous values.
 *
 * @param [in] average Pointer to exponential average from previous calculations.
 * @param [in] setpoint New input value to exponential average.
 * @param [in] factor Factor applied to exponential averaging.
 *
 * @return New exponential average.
 *
 */
double jstick_calc_exponential_average( double * const average,
                                        const double setpoint,
                                        const double factor );




#endif	/* JOYSTICK_H */
