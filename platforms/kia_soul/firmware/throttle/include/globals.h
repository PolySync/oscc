#ifndef _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_
#define _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_


#include "DAC_MCP49xx.h"
#include "mcp_can.h"

#include "throttle_control.h"


#define PIN_DAC_CHIP_SELECT ( 9 )
#define PIN_CAN_CHIP_SELECT ( 10 )
#define PIN_ACCELERATOR_POSITION_SENSOR_HIGH ( A0 )
#define PIN_ACCELERATOR_POSITION_SENSOR_LOW ( A1 )
#define PIN_ACCELERATOR_POSITION_SPOOF_HIGH ( A2 )
#define PIN_ACCELERATOR_POSITION_SPOOF_LOW ( A3 )
#define PIN_SPOOF_ENABLE ( 6 )

#define PARAM_ACCELERATOR_OVERRIDE_THRESHOLD_IN_DECIBARS ( 1000.0 )
#define PARAM_COMMAND_TIMEOUT_IN_MSEC ( 250 )


#ifdef GLOBAL_DEFINED
    DAC_MCP49xx dac( DAC_MCP49xx::MCP4922, PIN_DAC_CHIP_SELECT );
    MCP_CAN control_can( PIN_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern DAC_MCP49xx dac;
    extern MCP_CAN control_can;

    #define EXTERN extern
#endif


EXTERN uint32_t g_throttle_command_last_rx_timestamp;
EXTERN uint32_t g_throttle_report_last_tx_timestamp;

EXTERN kia_soul_throttle_control_state_s throttle_control_state;


#endif /* _OSCC_KIA_SOUL_THROTTLE_GLOBALS_H_ */
