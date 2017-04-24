#ifndef _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_
#define _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_


typedef struct
{
    float pressure;
} kia_soul_accumulator_s;


// *****************************************************
// Function:    accumulator_init
//
// Purpose:     Initializes the accumulator
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void accumulator_init( );


// *****************************************************
// Function:    accumulator_turn_pump_off
//
// Purpose:     Turn off accumulator pump
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void accumulator_turn_pump_off( );


// *****************************************************
// Function:    accumulator_turn_pump_on
//
// Purpose:     Turn on accumulator pump
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void accumulator_turn_pump_on( );


// *****************************************************
// Function:    accumulator_maintain_pressure
//
// Purpose:     Turn accumulator pump on or off
//              to maintain pressure
//
// Returns:     void
//
// Parameters:  void
//
// *****************************************************
void accumulator_maintain_pressure( );


#endif
