#include "communications.h"
#include "brake_control.h"
#include "can_protocols/brake_can_protocol.h"
#include "can_protocols/fault_can_protocol.h"
#include "vehicles.h"
#include "globals.h"


/** Define the module name under test
 *
 * In order to implement reusable test fixtures and steps, those fixtures
 * need the name of the module under test defined.
 *
 * \sa firmware/common/testing/step_definitions/common.cpp
 */
#define FIRMWARE_NAME "brake"


/** Define aliases to the brake control state
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
#define g_vcm_control_state g_brake_control_state


/** Match the oscc module fault checking naming convention
 *
 * The other oscc modules define a `check_for_faults()` function, but the
 * petrol brake module doesn't match that convention. In order to reuse the
 * common testing step definitions we define this function-like macro to
 * mask this variation.
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
#define check_for_faults() (check_for_sensor_faults())


extern volatile brake_control_state_s g_brake_control_state;


/** Define the origin ID that brake faults should be associated with
 *
 * \sa firmware/common/testing/step_definitions/fault_checking.cpp
 */
const int MODULE_FAULT_ORIGIN_ID = FAULT_ORIGIN_BRAKE;
