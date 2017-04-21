#ifndef _KIA_SOUL_STEERING_GLOBALS_H_
#define _KIA_SOUL_STEERING_GLOBALS_H_


#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"
#include "PID.h"

#include "steering_module.h"


/*******************************************************************************
*   WARNING
*
*   The ranges selected to do steering control are carefully tested to
*   ensure that a torque is not requested that the vehicles steering motor
*   cannot handle. By changing any of this code you risk attempting to actuate
*   a torque outside of the vehicles valid range. Actuating a torque outside of
*   the vehicles valid range will, at best, cause the vehicle to go into an
*   unrecoverable fault state. Clearing this fault state requires one of Kia's
*   native diagnostics tools, and someone who knows how to clear DTC codes with
*   said tool.
*
*   It is NOT recommended to modify any of the existing control ranges, or
*   gains, without expert knowledge.
*******************************************************************************/

#define PARAM_STEERING_ANGLE_RATE_MAX ( 1000.0 ) /* Maximum rate of change of steering wheel angle */
#define PARAM_STEERING_WHEEL_CUTOFF_THRESHOLD ( 3000 )
#define PARAM_PID_PROPORTIONAL_GAIN ( 0.3 ) /* Proportional gain for PID controller */
#define PARAM_PID_INTEGRAL_GAIN ( 1.3 ) /* Integral gain for PID controller */
#define PARAM_PID_DIFFERENTIAL_GAIN ( 0.03 ) /* Derivative gain for PID controller */
#define PARAM_PID_WINDUP_GUARD ( 1500 ) /* Windup guard of the PID controller */
#define PARAM_RX_TIMEOUT ( 250 ) /* Amount of time when system is considered unresponsive (milliseconds) */

#define PIN_DAC_CS ( 9 ) /* DAC chip select */
#define PIN_CAN_CS ( 10 ) /* CAN chip select */
#define PIN_TORQUE_POS_SENSOR_HIGH ( A0 ) /* High signal from torque position sensor */
#define PIN_TORQUE_POS_SENSOR_LOW ( A1 ) /* Low signal from torque position sensor */
#define PIN_TORQUE_POS_SPOOF_HIGH ( A2 ) /* High signal of spoof output */
#define PIN_TORQUE_POS_SPOOF_LOW ( A3 ) /* Low signal of spoof output */
#define PIN_SPOOF_ENABLE ( 6 ) /* Relay enable for spoofed torque values */


#ifdef GLOBAL_DEFINED
    DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, PIN_DAC_CS );
    MCP_CAN can( PIN_CAN_CS );

    #define EXTERN
#else
    extern DAC_MCP49xx dac;
    extern MCP_CAN can;

    #define EXTERN extern
#endif


EXTERN can_frame_s rx_frame_steering_command;
EXTERN can_frame_s tx_frame_steering_report;
EXTERN kia_soul_steering_override_flags_s override_flags;
EXTERN kia_soul_steering_state_s steering_state;
EXTERN kia_soul_control_state_s control_state;
EXTERN PID pid;
EXTERN uint8_t torque_sum;


#endif
