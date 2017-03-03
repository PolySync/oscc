/************************************************************************/
/* The MIT License (MIT) */
/* ===================== */

/* Copyright (c) 2017 PolySync Technologies, Inc.  All Rights Reserved. */

/* Permission is hereby granted, free of charge, to any person */
/* obtaining a copy of this software and associated documentation */
/* files (the “Software”), to deal in the Software without */
/* restriction, including without limitation the rights to use, */
/* copy, modify, merge, publish, distribute, sublicense, and/or sell */
/* copies of the Software, and to permit persons to whom the */
/* Software is furnished to do so, subject to the following */
/* conditions: */

/* The above copyright notice and this permission notice shall be */
/* included in all copies or substantial portions of the Software. */

/* THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES */
/* OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT */
/* HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, */
/* WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING */
/* FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR */
/* OTHER DEALINGS IN THE SOFTWARE. */
/************************************************************************/

/**
 * @file oscc_interface.h
 * @brief OSCC interface - The main command* functions and the
 *        update function should be called on at least a 50ms
 *        period.  The expectation is that if there is not some
 *        kind of communication from the controller to the OSCC
 *        modules in that time, then the OSCC modules will
 *        disable and return control back to the driver.
 */


#ifndef OSCC_INTERFACE_H
#define OSCC_INTERFACE_H


/**
 * @brief Initialize the OSCC interface - This call must occur
 *        first in order to use the interface at all.  If this
 *        call does not come first, all other calls will return
 *        ERROR.
 *
 * @param [in] channel - for now, the CAN channel to use when
 *        communicating with the OSCC modules
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_init( int channel );


/**
 * @brief Close the OSCC interface
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
void oscc_interface_close( );


/**
 * @brief Enable the OSCC interface - This function specifically
 *        tells the OSCC modules to engage the control
 *        functionality.  After this command goes through, the
 *        vehicle is being run via remote control.
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_enable( );


/**
 * @brief Disable the OSCC interface - Disables the remote
 *        control and returns vehicle control to the driver.
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_disable( );


/**
 * @brief Set the default values on the OSCC interface - This is
 *        mostly a preparatory function for transferring control
 *        between the OSCC modules and the driver
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_set_defaults( );


/**
 * @brief Disable only the braking module
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_disable_brakes( );


/**
 * @brief Disable only the throttle module
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_disable_throttle( );


/**
 * @brief Disable only the steering module
 *
 * @param [void]
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_disable_steering( );


/**
 * @brief Send a brake value to the braking module - This
 *        command sends a value to the OSCC braking module on
 *        how hard to apply the brakes.
 *
 * @param [in] brake_setpoint - value to set the brakes to.  The
 *        possible range goes from 0 to 52428 (16 bits)
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_command_brakes( unsigned int brake_setpoint );


/**
 * @brief Send a throttle value to the throttle module - This
 *        command sends a value to the OSCC throttle module to
 *        control how much fuel to send to the engine
 *
 * @param [in] throttle_setpoint - value to set the throttle
 *        to. The possible range goes from 0 to 19660 (16 bits)
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_command_throttle( unsigned int throttle_setpoint );


/**
 * @brief Send steering values to the steering module - This
 *        command sends an angle and turn rate to the OSCC
 *        steering module to control how much and how fast to
 *        turn the steering wheel
 *
 * @param [in] angle - steering wheel angle to set - 
 *        possible range goes from -4700 to 4700 degrees (signed
 *        16 bit value)
 * 
 *        [in] rate - how fast to turn the steering wheel -
 *        possible range goes from 20 to 254 degrees/sec
 *        (unsigned 16 bit value)
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_command_steering( int angle, unsigned int rate );

/**
 * @brief OSCC status message - the primary status from the
 *        OSCC modules is whether or not the modules have
 *        detected a driver override of the OSCC control. If any
 *        of the modules have been overridded by the driver,
 *        this status will indicate that
 * 
 *        This is a polled call that must be read at 50ms or
 *        faster.
 *
 * @param [in/out] override - If any of the OSCC modules have
 *        detected a driver override, this value will contain a
 *        1 after this function returns.  Otherwise the return
 *        is 0.
 *
 * @return ERROR or NOERR
 *
 */
int oscc_interface_update_status( int* override );

#endif /* OSCC_INTERFACE_H */

