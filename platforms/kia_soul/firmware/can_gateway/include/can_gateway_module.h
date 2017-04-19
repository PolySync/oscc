#ifndef _OSCC_KIA_SOUL_CAN_GATEWAY_MODULE_H_
#define _OSCC_KIA_SOUL_CAN_GATEWAY_MODULE_H_


typedef struct
{
    uint8_t obd_can_cs = 9; /* OBD CAN chip select */
    uint8_t control_can_cs = 10; /* Control CAN chip select */
    uint8_t status_led = 13; /* Status LED */
} kia_soul_can_gateway_pins_s;


typedef struct
{
    kia_soul_can_gateway_pins_s pins; /* Pin assignments of the CAN gateway module */
} kia_soul_can_gateway_module_s;

#endif
