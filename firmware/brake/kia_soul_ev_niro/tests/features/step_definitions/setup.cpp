#include "communications.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "globals.h"

#define FIRMWARE_NAME "brake"
#define g_vcm_control_state g_brake_control_state

extern volatile brake_control_state_s g_brake_control_state;
