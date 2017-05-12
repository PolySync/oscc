/**
 * @file time.h
 * @brief Time utilities.
 *
 */


#ifndef _OSCC_TIME_H_
#define _OSCC_TIME_H_


#include <Arduino.h>
#include <stdint.h>


/*
 * @brief Get current system timestamp in milliseconds.
 *
 */
#define GET_TIMESTAMP_MS() ((uint32_t) millis())

/*
 * @brief Get current system timestamp in microseconds.
 *
 */
#define GET_TIMESTAMP_US() ((uint32_t) micros())

/*
 * @brief Delay execution in milliseconds.
 *
 */
#define SLEEP_MS(x) delay(x)


// ****************************************************************************
// Function:    get_time_delta
//
// Purpose:     Calculate the difference between timestamp_a and timestamp_b.
//
// Returns:     uint32_t - difference between timestamp_a and timestamp_b
//
// Parameters:  [in] timestamp_a - first timestamp for comparison
//              [in] timestamp_b - second timestamp for comparison
//
// ****************************************************************************
uint32_t get_time_delta(
        const uint32_t timestamp_a,
        const uint32_t timestamp_b);


// ****************************************************************************
// Function:    is_timeout
//
// Purpose:     Check if a timestamp is greater than a timeout period.
//
// Returns:     bool - true if a timeout has occurred
//
// Parameters:  [in] timestamp - time against which to compare
//              [in] timeout - timeout period to check against
//
// ****************************************************************************
bool is_timeout(
        const uint32_t timestamp_a,
        const uint32_t timestamp_b,
        const int timeout );


#endif /* _OSCC_TIME_H_ */
