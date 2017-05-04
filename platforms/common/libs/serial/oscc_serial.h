/**
 * @file serial.h
 * @brief Serial interface.
 *
 */


#ifndef _OSCC_SERIAL_H_
#define _OSCC_SERIAL_H_


/*
 * @brief Serial baudrate.
 *
 */
#define SERIAL_BAUD (115200)


// ****************************************************************************
// Function:    init_serial
//
// Purpose:     Initializes the serial port communication.
//
// Returns:     void
//
// Parameters:  void
//
// ****************************************************************************
void init_serial( void );



#endif /* _OSCC_SERIAL_H_ */
