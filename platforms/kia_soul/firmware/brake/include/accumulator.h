#ifndef _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_
#define _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_


#include "brake_module.h"


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
void accumulator_init( kia_soul_brake_module_s *brake_module );


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
void accumulator_turn_pump_off( kia_soul_brake_module_s *brake_module );


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
void accumulator_turn_pump_on( kia_soul_brake_module_s *brake_module );


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
void accumulator_maintain_pressure( kia_soul_brake_module_s *brake_module );


#endif
