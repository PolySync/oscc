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
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif


#endif /* _OSCC_DEBUG_H_ */
