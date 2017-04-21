#ifndef _KIA_SOUL_THROTTLE_GLOBALS_H_
#define _KIA_SOUL_THROTTLE_GLOBALS_H_


#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"

#include "throttle_module.h"


#define PIN_DAC_CS ( 9 ) /* DAC chip select */
#define PIN_CAN_CS ( 10 ) /* CAN chip select */
#define PIN_ACCEL_POS_SENSOR_HIGH ( A0 ) /* High signal from accelerator position sensor */
#define PIN_ACCEL_POS_SENSOR_LOW ( A1 ) /* Low signal from accelerator position sensor */
#define PIN_ACCEL_POS_SPOOF_HIGH ( A2 )/* High signal of spoof output */
#define PIN_ACCEL_POS_SPOOF_LOW ( A3 ) /* Low signal of spoof output */
#define PIN_SPOOF_ENABLE ( 6 ) /* Relay enable for spoofed accel values */

#define PARAM_ACCEL_OVERRIDE_THRESHOLD ( 1000.0 ) /* Threshhold to detect when a person is pressing accelerator */
#define PARAM_RX_TIMEOUT ( 250 ) /* Amount of time when system is considered unresponsive (milliseconds) */


#ifdef GLOBAL_DEFINED
    DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, PIN_DAC_CS );
    MCP_CAN can( PIN_CAN_CS );

    #define EXTERN
#else
    extern DAC_MCP49xx dac;
    extern MCP_CAN can;

    #define EXTERN extern
#endif


EXTERN can_frame_s rx_frame_throttle_command;
EXTERN can_frame_s tx_frame_throttle_report;
EXTERN kia_soul_throttle_override_flags_s override_flags;
EXTERN kia_soul_throttle_state_s throttle_state;
EXTERN kia_soul_throttle_control_state_s control_state;


#endif /* _KIA_SOUL_THROTTLE_GLOBALS_H_ */
