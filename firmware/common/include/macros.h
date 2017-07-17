/**
 * @file macros.h
 * @brief Global macros for joystick project.
 *
 */


#ifndef MACROS_H
#define MACROS_H




/**
 * @brief Error macro.
 *
 */
#define ERROR 0


/**
 * @brief Macro indicating no error.
 *
 */
#define NOERR 1


/**
 * @brief Macro indicating a warning but not an error.
 *
 */
#define UNAVAILABLE 2


/**
 * @brief Math macro: constrain(amount, low, high).
 *
 */
#define m_constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))




#endif /* MACROS_H */

