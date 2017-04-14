#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
    #warning "DEBUG defined"
    #define DEBUG_PRINT(x) Serial.print(x)
    #define DEBUG_PRINTLN(x) Serial.println(x)
#else
    #define DEBUG_PRINT(x)
    #define DEBUG_PRINTLN(x)
#endif

#endif
