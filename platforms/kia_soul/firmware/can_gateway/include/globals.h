#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_


#include "mcp_can.h"
#include "gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"


#define PIN_OBD_CAN_CHIP_SELECT ( 9 )
#define PIN_CONTROL_CAN_CHIP_SELECT ( 10 )


#ifdef GLOBAL_DEFINED
    MCP_CAN obd_can( PIN_OBD_CAN_CHIP_SELECT );
    MCP_CAN control_can( PIN_CONTROL_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN obd_can;
    extern MCP_CAN control_can;

    #define EXTERN extern
#endif


EXTERN oscc_report_heartbeat_s tx_heartbeat;
EXTERN oscc_report_chassis_state_1_s tx_chassis_state_1;
EXTERN oscc_report_chassis_state_2_s tx_chassis_state_2;

EXTERN uint32_t obd_steering_wheel_angle_rx_timestamp;
EXTERN uint32_t obd_wheel_speed_rx_timestamp;
EXTERN uint32_t obd_brake_pressure_rx_timestamp;
EXTERN uint32_t obd_turn_signal_rx_timestamp;


#endif
