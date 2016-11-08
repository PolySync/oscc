/**
 * @file macros.h
 * @brief Global macros for joystick project.
 *
 */

#ifndef MACROS_H
#define MACROS_H

#define ERROR 0

#define NOERR 1

#define UNAVAILABLE 2

/**
 * @brief Math macro: constrain(amount, low, high).
 *
 */
#define m_constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))

#endif /* MACROS_H */

