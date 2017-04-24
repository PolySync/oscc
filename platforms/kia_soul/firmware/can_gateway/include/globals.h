#ifndef _KIA_SOUL_CAN_GATEWAY_GLOBALS_H_
#define _KIA_SOUL_CAN_GATEWAY_GLOBALS_H_


#include "mcp_can.h"
#include "can.h"


#define PIN_OBD_CAN_CS ( 9 ) /* OBD CAN chip select */
#define PIN_CONTROL_CAN_CS ( 10 ) /* Control CAN chip select */
#define PIN_STATUS_LED ( 13 ) /* Status LED */


#ifdef GLOBAL_DEFINED
    MCP_CAN obd_can( PIN_OBD_CAN_CS );
    MCP_CAN control_can( PIN_CONTROL_CAN_CS );

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
