#include "communications.h"
#include "throttle_control.h"
#include "can_protocols/throttle_can_protocol.h"
#include "globals.h"

#define FIRMWARE_NAME "throttle"
#define g_vcm_control_state g_throttle_control_state

extern volatile throttle_control_state_s g_throttle_control_state;
