/**
 * @file debug.h
 * @brief Debug Utilities.
 *
 */


#ifndef _OSCC_DEBUG_H_
#define _OSCC_DEBUG_H_


#include <Arduino.h>


#ifdef DEBUG
    #warning "DEBUG defined"
    #define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
    #define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
    #define DEBUG_PRINT(...)
    #define DEBUG_PRINTLN(...)
#endif


#endif /* _OSCC_DEBUG_H_ */
