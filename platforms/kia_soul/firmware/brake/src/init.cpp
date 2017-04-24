#include "serial.h"
#include "can.h"

#include "globals.h"
#include "init.h"


void init_interfaces( void )
{
    #ifdef DEBUG
    init_serial( );
    #endif

    init_can( can );
}
