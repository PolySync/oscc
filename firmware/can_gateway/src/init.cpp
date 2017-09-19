/**
 * @file init.cpp
 *
 */


#include "debug.h"
#include "globals.h"
#include "init.h"
#include "oscc_can.h"
#include "oscc_serial.h"


void init_communication_interfaces( void )
{
    #ifdef DEBUG
    init_serial();
    #endif

    DEBUG_PRINT( "init OBD CAN - ");
    init_can( g_obd_can );

    DEBUG_PRINT( "init Control CAN - ");
    init_can( g_control_can );
}
