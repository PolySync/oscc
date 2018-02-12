#include "communications.h"
#include "steering_control.h"
#include "can_protocols/steering_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "globals.h"


/** Define the module name under test
 *
 * In order to implement reusable test fixtures and steps, those fixtures
 * need the name of the module under test defined.
 *
 * \sa firmware/common/testing/step_definitions/common.cpp
 */
#define FIRMWARE_NAME "steering"


/** Define aliases to the steering control state
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
#define g_vcm_control_state g_steering_control_state


extern volatile steering_control_state_s g_steering_control_state;


/** Define the origin ID that steering faults should be associated with
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
const int MODULE_FAULT_ORIGIN_ID = FAULT_ORIGIN_STEERING;
