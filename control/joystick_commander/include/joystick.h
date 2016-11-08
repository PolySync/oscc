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
#define JOYSTICK_DEVICE_HANDLE_INVALID (NULL)


/**
 * @brief Lowest joystick axis value.
 *
 */
#define JOYSTICK_AXIS_POSITION_MIN (-32768)


/**
 * @brief Highest joystick axis value.
 *
 */
#define JOYSTICK_AXIS_POSITION_MAX (32767)


/**
 * @brief Button state not pressed.
 *
 */
#define JOYSTICK_BUTTON_STATE_NOT_PRESSED (0)


/**
 * @brief Button state pressed.
 *
 */
#define JOYSTICK_BUTTON_STATE_PRESSED (1)




/**
 * @brief Joystick GUID.
 *
 * Implementation-dependent GUID
 *
 */
typedef struct
{
    //
    //
    unsigned char data[16]; /*!< Identifier data. */
    //
    //
    char ascii_string[64]; /*!< Description string. */
} joystick_guid_s;


/**
 * @brief Joystick device.
 *
 */
typedef struct
{
    //
    //
    void *handle; /*!< Device handle. */
    //
    //
    joystick_guid_s guid; /*!< Device GUID. */
} joystick_device_s;




/**
 * @brief Initialize joystick subsystem.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int jstick_init_subsystem( void );


/**
 * @brief Release joystick subsystem.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
void jstick_release_subsystem( void );


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
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int jstick_get_guid_at_index(
        const unsigned long device_index,
        joystick_guid_s * const guid );


/**
 * @brief Open joystick device.
 *
 * @param [in] device_index Device index in the subsystem.
 * @param [out] jstick A pointer to \ref joystick_device_s which receives the configuration.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int jstick_open(
        const unsigned long device_index,
        joystick_device_s * const jstick );


/**
 * @brief Close joystick device.
 *
 * @param [out] jstick A pointer to \ref joystick_device_s which is to be closed.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
void jstick_close(
    joystick_device_s * const jstick );


/**
 * @brief Update joystick device.
 *
 * @param [out] jstick A pointer to \ref joystick_device_s which receives the update.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 * \li \ref DTC_USAGE if arguments invalid.
 * \li \ref DTC_UNAVAILABLE if joystick is not accessible.
 *
 */
int jstick_update(
    joystick_device_s * const jstick );


/**
 * @brief Get joystick axis value.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the configuration.
 * @param [in] axis_index Axis index.
 * @param [out] position Current axis value.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int jstick_get_axis(
        joystick_device_s * const jstick,
        const unsigned long axis_index,
        int * const position );


/**
 * @brief Get joystick button state.
 *
 * @param [in] jstick A pointer to \ref joystick_device_s which specifies the configuration.
 * @param [in] button_index Button index.
 * @param [out] state Current button state.
 *
 * @return DTC code:
 * \li \ref DTC_NONE (zero) if success.
 *
 */
int jstick_get_button(
        joystick_device_s * const jstick,
        const unsigned long button_index,
        unsigned int * const state );


/**
 * @brief Map axis value from one range to another.
 *
 * @param [in] position Input value to map.
 * @param [in] range_min Output minimum range.
 * @param [in] range_max Output maximum range.
 *
 * @return position mapped to the range of min:max.
 *
 */
double jstick_normalize_axis_position(
        const int position,
        const double range_min,
        const double range_max );




#endif	/* JOYSTICK_H */
