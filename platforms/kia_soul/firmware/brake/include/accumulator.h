/**
 * @file accumulator.h
 * @brief Accumulator functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_
#define _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_


/*
 * @brief Minimum accumulator presure. [decibars]
 *
 */
#define ACCUMULATOR_PRESSURE_MIN_IN_DECIBARS ( 777.6 )

/*
 * @brief Maximum accumulator pressure. [decibars]
 *
 */
#define ACCUMULATOR_PRESSURE_MAX_IN_DECIBARS ( 878.0 )


// ****************************************************************************
// Function:    accumulator_init
//
// Purpose:     Initializes the accumulator.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void accumulator_init( void );


// ****************************************************************************
// Function:    accumulator_turn_pump_off
//
// Purpose:     Turns off the accumulator pump.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void accumulator_turn_pump_off( void );


// ****************************************************************************
// Function:    accumulator_turn_pump_on
//
// Purpose:     Turns on the accumulator pump.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void accumulator_turn_pump_on( void );


// ****************************************************************************
// Function:    accumulator_maintain_pressure
//
// Purpose:     Turn accumulator pump on or off to maintain pressure
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void accumulator_maintain_pressure( void );


#endif /* _OSCC_KIA_SOUL_BRAKE_ACCUMULATOR_H_ */
