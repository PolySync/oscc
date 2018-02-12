#include "communications.h"
#include "throttle_control.h"
#include "can_protocols/throttle_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "globals.h"


/** Define the module name under test
 *
 * In order to implement reusable test fixtures and steps, those fixtures
 * need the name of the module under test defined.
 *
 * \sa firmware/common/testing/step_definitions/common.cpp
 */
#define FIRMWARE_NAME "throttle"


/** Define aliases to the throttle control state
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
#define g_vcm_control_state g_throttle_control_state


extern volatile throttle_control_state_s g_throttle_control_state;


/** Define the origin ID that throttle faults should be associated with
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
const int MODULE_FAULT_ORIGIN_ID = FAULT_ORIGIN_THROTTLE;
