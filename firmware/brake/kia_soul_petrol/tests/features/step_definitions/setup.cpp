#include "communications.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "vehicles.h"
#include "globals.h"

extern volatile brake_control_state_s g_brake_control_state;

#define FIRMWARE_NAME "brake"
#define g_vcm_control_state g_brake_control_state

#define check_for_faults() (check_for_sensor_faults())
