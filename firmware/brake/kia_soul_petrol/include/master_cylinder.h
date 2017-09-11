/**
 * @file init.h
 * @brief Initialization functionality.
 *
 */


#ifndef _OSCC_BRAKE_MASTER_CYLINDER_H_
#define _OSCC_BRAKE_MASTER_CYLINDER_H_


typedef struct
{
    float sensor_1_pressure;
    float sensor_2_pressure;
} master_cylinder_pressure_s;


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


// ****************************************************************************
// Function:    master_cylinder_read_pressure
//
// Purpose:     Reads the pressure of the master cylinder.
//
// Returns:     void
//
// Parameters:  pressure - a structure containing the values of sensor 1 and
//                         sensor 2 of the master cylinder
//
// ****************************************************************************
void master_cylinder_read_pressure( master_cylinder_pressure_s * pressure );


#endif /* _OSCC_BRAKE_MASTER_CYLINDER_H_ */
