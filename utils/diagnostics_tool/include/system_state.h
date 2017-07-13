/**
* @file system_state.h
* @brief System state.
*
**/




#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H




#include <stdint.h>

#include "steering_module_state.h"
#include "throttle_module_state.h"
#include "brake_module_state.h"
#include "gateway_module_state.h"




// Module enabled.
#define CONTROL_ENABLED ( 1 )


// Module disabled.
#define CONTROL_DISABLED ( 0 )


// Module in an OK state
#define STATE_OK ( 2 )


// Module in an WARN state
#define STATE_WARN ( 1 )


// Module in an ERROR state
#define STATE_ERROR ( 0 )


// Module in a non expected state.
#define STATE_FAULT ( -1 )




/**
 * @brief System state information.
 *
 * Contains system state information for whole system.
 *
 */
typedef struct
{
    //
    //
    throttle_module_state_s throttle_module_state;
    //
    //
    steering_module_state_s steering_module_state;
    //
    //
    brake_module_state_s brake_module_state;
    //
    //
    gateway_module_state_s gateway_module_state;
    //
    //
    uint8_t overall_system_state;
    //
    //
    uint8_t overall_system_control_state;
} current_system_state_s;




//
int update_system_state();


//
void print_system_state();




#endif /* SYSTEM_STATE_H */
