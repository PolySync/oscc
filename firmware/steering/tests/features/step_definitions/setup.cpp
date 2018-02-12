#include "communications.h"
#include "steering_control.h"
#include "can_protocols/steering_can_protocol.h"
#include "globals.h"

#define FIRMWARE_NAME "steering"
#define g_vcm_control_state g_steering_control_state

extern volatile steering_control_state_s g_steering_control_state;
