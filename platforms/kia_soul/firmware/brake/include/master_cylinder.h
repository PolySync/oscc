#ifndef _OSCC_KIA_SOUL_BRAKE_MASTER_CYLINDER_H_
#define _OSCC_KIA_SOUL_BRAKE_MASTER_CYLINDER_H_


#include "brake_module.h"


// *****************************************************
// Function:    master_cylinder_open
//
// Purpose:     Open master cylinder solenoid
//
// Returns:     void
//
// Parameters:  brake_module - structure containing brake module information
//
// *****************************************************
void master_cylinder_open( kia_soul_brake_module_s *brake_module );

// *****************************************************
// Function:    master_cylinder_close
//
// Purpose:     Close master cylinder solenoid
//
// Returns:     void
//
// Parameters:  brake_module - structure containing brake module information
//
// *****************************************************
void master_cylinder_close( kia_soul_brake_module_s *brake_module );


// *****************************************************
// Function:    master_cylinder_init
//
// Purpose:     Initializes the master cylinder solenoid
//
// Returns:     void
//
// Parameters:  brake_module - structure containing brake module information
//
// *****************************************************
void master_cylinder_init( kia_soul_brake_module_s *brake_module );


#endif
