#include "communications.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "globals.h"

extern volatile brake_control_state_s g_brake_control_state;

#define FIRMWARE_NAME "brake"
#define g_vcm_control_state g_brake_control_state
#include "../../common/testing/step_definitions/common.cpp"
#include "../../common/testing/step_definitions/vcm.cpp"
