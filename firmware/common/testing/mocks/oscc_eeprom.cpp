#include <string.h>

#include "oscc_eeprom.h"


static uint8_t mock_eeprom[1024] = {0};

void oscc_eeprom_write_u8(
    oscc_config_u8_name_t name,
    uint8_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U8_EEPROM_INDEX
        + ( sizeof(uint8_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


uint8_t oscc_eeprom_read_u8(
    oscc_config_u8_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U8_EEPROM_INDEX
        + ( sizeof(uint8_t) * name );

    uint8_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_u16(
    oscc_config_u16_name_t name,
    uint16_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U16_EEPROM_INDEX
        + ( sizeof(uint16_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


uint16_t oscc_eeprom_read_u16(
    oscc_config_u16_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U16_EEPROM_INDEX
        + ( sizeof(uint16_t) * name );

    uint16_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_u32(
    oscc_config_u32_name_t name,
    uint32_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U32_EEPROM_INDEX
        + ( sizeof(uint32_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


uint32_t oscc_eeprom_read_u32(
    oscc_config_u32_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_U32_EEPROM_INDEX
        + ( sizeof(uint32_t) * name );

    uint32_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_i8(
    oscc_config_i8_name_t name,
    int8_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I8_EEPROM_INDEX
        + ( sizeof(int8_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


int8_t oscc_eeprom_read_i8(
    oscc_config_i8_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I8_EEPROM_INDEX
        + ( sizeof(int8_t) * name );

    int8_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_i16(
    oscc_config_i16_name_t name,
    int16_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I16_EEPROM_INDEX
        + ( sizeof(int16_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


int16_t oscc_eeprom_read_i16(
    oscc_config_i16_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I16_EEPROM_INDEX
        + ( sizeof(int16_t) * name );

    int16_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_i32(
    oscc_config_i32_name_t name,
    int32_t value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I32_EEPROM_INDEX
        + ( sizeof(int32_t) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


int32_t oscc_eeprom_read_i32(
    oscc_config_i32_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_I32_EEPROM_INDEX
        + ( sizeof(int32_t) * name );

    int32_t value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}


void oscc_eeprom_write_f32(
    oscc_config_f32_name_t name,
    float value )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_F32_EEPROM_INDEX
        + ( sizeof(float) * name );

    memcpy( (void *) (mock_eeprom+address), &value, sizeof(value) );
}


float oscc_eeprom_read_f32(
    oscc_config_f32_name_t name )
{
    // EEPROM address is offset by the configuration parameter's location in the enum
    uint32_t address =
        OSCC_CONFIG_F32_EEPROM_INDEX
        + ( sizeof(float) * name );

    float value;

    memcpy( &value, (void *) (mock_eeprom+address), sizeof(value) );

    return value;
}
