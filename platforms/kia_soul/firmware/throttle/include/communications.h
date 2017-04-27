#ifndef _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_
#define _OSCC_KIA_SOUL_THROTTLE_COMMUNICATIONS_H_


#include "can.h"


void publish_reports( void );

void check_for_controller_command_timeout( void );

void check_for_incoming_message( void );


#endif
