#ifndef _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_
#define _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_


#include <Arduino.h>
#include "DAC_MCP49xx.h"
#include "mcp_can.h"
#include "can.h"

#include "throttle_module.h"


#define PIN_DAC_CHIP_SELECT ( 9 )
#define PIN_CAN_CHIP_SELECT ( 10 )
#define PIN_ACCELERATOR_POSITION_SENSOR_HIGH ( A0 )
#define PIN_ACCELERATOR_POSITION_SENSOR_LOW ( A1 )
#define PIN_ACCELERATOR_POSITION_SPOOF_HIGH ( A2 )
#define PIN_ACCELERATOR_POSITION_SPOOF_LOW ( A3 )
#define PIN_SPOOF_ENABLE ( 6 )

#define PARAM_ACCELERATOR_OVERRIDE_THRESHOLD_IN_DECIBARS ( 1000.0 )
#define PARAM_RX_TIMEOUT_IN_MSEC ( 250 )


#ifdef GLOBAL_DEFINED
    DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, PIN_DAC_CHIP_SELECT );
    MCP_CAN can( PIN_CAN_CHIP_SELECT );

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


#endif /* _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_ */
