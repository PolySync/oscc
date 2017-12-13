/**
 * @file oscc_eeprom.h
 * @brief EEPROM interface.
 *
 */


#ifndef _OSCC_EEPROM_H_
#define _OSCC_EEPROM_H_

#include <stdint.h>

#include "can_protocols/config_can_protocol.h"


// ****************************************************************************
// Function:    oscc_eeprom_write_u8
//
// Purpose:     Writes a 8-bit unsigned integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u8_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_u8(
    oscc_config_u8_name_t name,
    uint8_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_u8
//
// Purpose:     Reads a 8-bit unsigned integer from EEPROM.
//
// Returns:     uint8_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u8_name_t.
//
// ****************************************************************************
uint8_t oscc_eeprom_read_u8(
    oscc_config_u8_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_u16
//
// Purpose:     Writes a 16-bit unsigned integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u16_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_u16(
    oscc_config_u16_name_t name,
    uint16_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_u16
//
// Purpose:     Reads a 16-bit unsigned integer from EEPROM.
//
// Returns:     uint16_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u16_name_t.
//
// ****************************************************************************
uint16_t oscc_eeprom_read_u16(
    oscc_config_u16_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_u32
//
// Purpose:     Writes a 32-bit unsigned integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u32_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_u32(
    oscc_config_u32_name_t name,
    uint32_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_u32
//
// Purpose:     Reads a 32-bit unsigned integer from EEPROM.
//
// Returns:     uint32_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_u32_name_t.
//
// ****************************************************************************
uint32_t oscc_eeprom_read_u32(
    oscc_config_u32_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_i8
//
// Purpose:     Writes a 8-bit signed integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i8_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_i8(
    oscc_config_i8_name_t name,
    int8_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_i8
//
// Purpose:     Reads a 8-bit signed integer from EEPROM.
//
// Returns:     int8_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i8_name_t.
//
// ****************************************************************************
int8_t oscc_eeprom_read_i8(
    oscc_config_i8_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_i16
//
// Purpose:     Writes a 16-bit signed integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i16_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_i16(
    oscc_config_i16_name_t name,
    int16_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_i16
//
// Purpose:     Reads a 16-bit signed integer from EEPROM.
//
// Returns:     int16_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i16_name_t.
//
// ****************************************************************************
int16_t oscc_eeprom_read_i16(
    oscc_config_i16_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_i32
//
// Purpose:     Writes a 32-bit signed integer to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i32_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_i32(
    oscc_config_i32_name_t name,
    int32_t value );


// ****************************************************************************
// Function:    oscc_eeprom_read_i32
//
// Purpose:     Reads a 32-bit signed integer from EEPROM.
//
// Returns:     int32_t - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_i32_name_t.
//
// ****************************************************************************
int32_t oscc_eeprom_read_i32(
    oscc_config_i32_name_t name );


// ****************************************************************************
// Function:    oscc_eeprom_write_f32
//
// Purpose:     Writes a 32-bit float to EEPROM.
//
// Returns:     void
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_f32_name_t.
//              [in] value - Value to write to the EEPROM.
//
// ****************************************************************************
void oscc_eeprom_write_f32(
    oscc_config_f32_name_t name,
    float value );


// ****************************************************************************
// Function:    oscc_eeprom_read_f32
//
// Purpose:     Reads a 32-bit float from EEPROM.
//
// Returns:     float - Value read from EEPROM.
//
// Parameters:  [in] name - Name of config parameter of type oscc_config_f32_name_t.
//
// ****************************************************************************
float oscc_eeprom_read_f32(
    oscc_config_f32_name_t name );


#endif /* _OSCC_EEPROM_H_ */
