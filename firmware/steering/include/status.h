/**
 * @file status.h
 * @brief Control of the status (LED) system.
 *
 */


#ifndef _OSCC_STATUS_H_
#define _OSCC_STATUSL_H_


#include <stdint.h>



// ****************************************************************************
// Function:    init_status
//
// Purpose:     Configured status LED pins as output and turns them on.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_status( void );

// ****************************************************************************
// Function:    status_ledsOff
//
// Purpose:     Turns off both status LEDs
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void status_ledsOff( void);

// ****************************************************************************
// Function:    status_ledsOn
//
// Purpose:     Turns on both status LEDs
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void status_ledsOn( void );

// ****************************************************************************
// Function:    status_setRedLed
//
// Purpose:     Turns the Red LED on or off depending on the value of state.
//
// Returns:     void
//
// Parameters:  state - If true, turn LED on
//
// ****************************************************************************
void status_setRedLed(uint8_t state);

// ****************************************************************************
// Function:    status_setGreenLed
//
// Purpose:     Turns the Green LED on or off depending on the value of state.
//
// Returns:     void
//
// Parameters:  state - If true, turn LED on
//
// ****************************************************************************
void status_setGreenLed(uint8_t state);




#endif /* _OSCC_STATUS_H_ */
