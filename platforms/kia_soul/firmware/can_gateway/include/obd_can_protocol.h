/**
 * @file obd_can_protocol.h
 * @brief Kia Soul OBDII CAN Protocol.
 *
 */


#ifndef _OSCC_KIA_SOUL_OBD_CAN_PROTOCOL_H_
#define _OSCC_KIA_SOUL_OBD_CAN_PROTOCOL_H_


#include <stdint.h>


//
#define OSCC_HEARTBEAT_WARN_KIA_STATUS1_TIMEOUT (0x0100)


//
#define OSCC_HEARTBEAT_WARN_KIA_STATUS2_TIMEOUT (0x0200)


//
#define OSCC_HEARTBEAT_WARN_KIA_STATUS3_TIMEOUT (0x0400)


//
#define OSCC_HEARTBEAT_WARN_KIA_STATUS4_TIMEOUT (0x0800)


//
#define KIA_CCAN_STATUS1_MESSAGE_ID (0x2B0)


//
#define KIA_CCAN_STATUS2_MESSAGE_ID (0x4B0)


//
#define KIA_CCAN_STATUS3_MESSAGE_ID (0x220)


//
#define KIA_CCAN_STATUS4_MESSAGE_ID (0x18)


//
#define KIA_CCAN_STATUS4_TURN_SIGNAL_LEFT (0x0C)


//
#define KIA_CCAN_STATUS4_TURN_SIGNAL_RIGHT (0x0A)


// ms
#define KIA_CCAN_STATUS1_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS2_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS3_RX_WARN_TIMEOUT (50)


// ms
#define KIA_CCAN_STATUS4_RX_WARN_TIMEOUT (500)




// 0x2B0
// signals: ...
typedef struct
{
    //
    // 1/10th of a degree per bit
    int16_t steering_angle;
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    //
    uint16_t reserved_2;
} kia_obd_status1_data_s;


// 0x4B0
// signals: ...
typedef struct
{
    //
    // 1/128 mph per bit
    int16_t wheel_speed_lf;
    //
    //
    int16_t wheel_speed_rf;
    //
    //
    int16_t wheel_speed_lr;
    //
    //
    int16_t wheel_speed_rr;
} kia_obd_status2_data_s;


// 0x220
// signals: ...
typedef struct
{
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    // 1/10th of a bar per bit
    int16_t master_cylinder_pressure;
    //
    //
    uint16_t reserved_2;
} kia_obd_status3_data_s;


// 0x18
// signals: ...
typedef struct
{
    //
    //
    uint16_t reserved_0;
    //
    //
    uint16_t reserved_1;
    //
    //
    uint8_t reserved_2;
    //
    //
    uint8_t reserved_3 : 4;
    //
    //
    uint8_t turn_signal_flags : 4;
    //
    //
    uint16_t reserved_4;
} kia_obd_status4_data_s;


#endif
