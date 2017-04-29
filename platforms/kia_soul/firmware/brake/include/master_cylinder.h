/**
 * @file init.h
 * @brief Initialization functionality.
 *
 */


#ifndef _OSCC_KIA_SOUL_BRAKE_MASTER_CYLINDER_H_
#define _OSCC_KIA_SOUL_BRAKE_MASTER_CYLINDER_H_


// ****************************************************************************
// Function:    master_cylinder_open
//
// Purpose:     Open the master cylinder solenoid.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void master_cylinder_open( void );


// ****************************************************************************
// Function:    master_cylinder_close
//
// Purpose:     Close the master cylinder solenoid.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void master_cylinder_close( void );


// ****************************************************************************
// Function:    master_cylinder_init
//
// Purpose:     Initialize the master cylinder solenoid.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void master_cylinder_init( void );


#endif /* _OSCC_KIA_SOUL_BRAKE_MASTER_CYLINDER_H_ */
