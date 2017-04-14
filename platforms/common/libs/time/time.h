#ifndef _OSCC_TIME_H_
#define _OSCC_TIME_H_


//
#define GET_TIMESTAMP_MS() ((uint32_t) millis())

//
#define GET_TIMESTAMP_US() ((uint32_t) micros())

//
#define SLEEP_MS(x) delay(x)


void get_update_time_delta_ms(
        const uint32_t time_in,
        uint32_t last_update_ms,
        uint32_t * const delta_out );


// *****************************************************
// Function:    timer_delta_ms
//
// Purpose:     Calculate the milliseconds between the current time and the
//              input and correct for the timer overflow condition
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] last_time - the last time sample
//              [in] current_time - pointer to store the current time
//
// *****************************************************
uint32_t timer_delta_ms( uint32_t last_time, uint32_t* current_time );


// *****************************************************
// Function:    timer_delta_us
//
// Purpose:     Calculate the microseconds between the` current time and the
//              input and correct for the timer overflow condition
//
// Returns:     uint32_t the time delta between the two inputs
//
// Parameters:  [in] last_time - the last time sample
//              [in] current_time - pointer to store the current time
//
// *****************************************************
uint32_t timer_delta_us( uint32_t last_time, uint32_t* current_time );


// *****************************************************
// Function:    check_rx_timeouts
//
// Purpose:     Check if time since last receive is greater than timeout period
//
// Returns:     bool - true if a timeout has occurred
//
// Parameters:  [in] last_rx_time - time of last receive
//
// *****************************************************
bool check_rx_timeouts( uint32_t last_rx_time );

#endif