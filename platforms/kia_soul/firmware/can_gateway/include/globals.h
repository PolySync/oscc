#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_


#include "mcp_can.h"
#include "can.h"


#define PIN_OBD_CAN_CHIP_SELECT ( 9 )
#define PIN_CONTROL_CAN_CHIP_SELECT ( 10 )
#define PIN_STATUS_LED ( 13 )


#ifdef GLOBAL_DEFINED
    MCP_CAN obd_can( PIN_OBD_CAN_CHIP_SELECT );
    MCP_CAN control_can( PIN_CONTROL_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN obd_can;
    extern MCP_CAN control_can;

    #define EXTERN extern
#endif


EXTERN can_frame_s tx_frame_heartbeat;
EXTERN can_frame_s tx_frame_chassis_state1;
EXTERN can_frame_s tx_frame_chassis_state2;
EXTERN can_frame_s rx_frame_kia_status1;
EXTERN can_frame_s rx_frame_kia_status2;
EXTERN can_frame_s rx_frame_kia_status3;
EXTERN can_frame_s rx_frame_kia_status4;


#endif
