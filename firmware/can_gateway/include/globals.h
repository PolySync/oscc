/**
 * @file globals.h
 * @brief Module globals.
 *
 */


#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_


#include "mcp_can.h"
#include "ssd1325.h"

#include "display.h"



/*
 * @brief Chip select pin of the Control CAN IC.
 *
 */
#define PIN_CONTROL_CAN_CHIP_SELECT ( 4 )

/*
 * @brief Chip select pin of the OBD CAN IC.
 *
 */
#define PIN_OBD_CAN_CHIP_SELECT ( 5 )

/*
 * @brief Chip select pin of the OBD CAN IC.
 *
 */
#define PIN_C_CAN_CHIP_SELECT ( 6 )

/*
 * @brief Chip select pin of the OBD CAN IC.
 *
 */
#define PIN_D_CAN_CHIP_SELECT ( 7 )



#ifdef GLOBAL_DEFINED
    MCP_CAN g_control_can( PIN_CONTROL_CAN_CHIP_SELECT );
    MCP_CAN g_obd_can( PIN_OBD_CAN_CHIP_SELECT );
    MCP_CAN g_c_can( PIN_C_CAN_CHIP_SELECT );
    MCP_CAN g_d_can( PIN_D_CAN_CHIP_SELECT );

    #define EXTERN
#else
    extern MCP_CAN g_obd_can;
    extern MCP_CAN g_control_can;
    extern MCP_CAN g_c_can;
    extern MCP_CAN g_d_can;

    #define EXTERN extern
#endif





#endif /* _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_ */
