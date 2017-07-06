/**
 * @file globals.h
 * @brief Module globals.
 *
 */


#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_


#include "mcp_can.h"
#include "gateway_can_protocol.h"
#include "chassis_state_can_protocol.h"
#include "ssd1325.h"

#include "display.h"


/*
 * @brief Chip select pin of the OBD CAN IC.
 *
 */
#define PIN_OBD_CAN_CHIP_SELECT ( 9 )

/*
 * @brief Chip select pin of the Control CAN IC.
 *
 */
#define PIN_CONTROL_CAN_CHIP_SELECT ( 10 )

/*
 * @brief SPI SCLK pin to display.
 *
 */
#define PIN_DISPLAY_SCLK ( 13 )

/*
 * @brief SPI MOSI pin to display.
 *
 */
#define PIN_DISPLAY_MOSI ( 11 )

/*
 * @brief SPI CS pin to display.
 *
 */
#define PIN_DISPLAY_CS ( 7 )

/*
 * @brief Reset pin to display.
 *
 */
#define PIN_DISPLAY_RESET ( 6 )

/*
 * @brief DC pin to display.
 *
 */
#define PIN_DISPLAY_DC ( 5 )

/*
 * @brief Pin of display button.
 *
 */
#define PIN_DISPLAY_BUTTON ( 4 )

/*
 * @brief Pin of good (green) display LED.
 *
 */
#define PIN_DISPLAY_LED_GOOD ( 8 )

/*
 * @brief Pin of warning (yellow) display LED.
 *
 */
#define PIN_DISPLAY_LED_WARNING ( 3 )

/*
 * @brief Pin of error (red) display LED.
 *
 */
#define PIN_DISPLAY_LED_ERROR ( 2 )


#ifdef GLOBAL_DEFINED
    MCP_CAN g_obd_can( PIN_OBD_CAN_CHIP_SELECT );
    MCP_CAN g_control_can( PIN_CONTROL_CAN_CHIP_SELECT );
    SSD1325 g_display( PIN_DISPLAY_MOSI, PIN_DISPLAY_SCLK, PIN_DISPLAY_DC, PIN_DISPLAY_RESET, PIN_DISPLAY_CS );

    #define EXTERN
#else
    extern MCP_CAN g_obd_can;
    extern MCP_CAN g_control_can;
    extern SSD1325 g_display;

    #define EXTERN extern
#endif


EXTERN oscc_report_heartbeat_s g_tx_heartbeat;
EXTERN oscc_report_chassis_state_1_s g_tx_chassis_state_1;
EXTERN oscc_report_chassis_state_2_s g_tx_chassis_state_2;
EXTERN kia_soul_gateway_display_state_s g_display_state;

EXTERN uint32_t g_obd_steering_wheel_angle_rx_timestamp;
EXTERN uint32_t g_obd_wheel_speed_rx_timestamp;
EXTERN uint32_t g_obd_brake_pressure_rx_timestamp;
EXTERN uint32_t g_obd_turn_signal_rx_timestamp;


#endif /* _OSCC_KIA_SOUL_CAN_GATEWAY_GLOBALS_H_ */
