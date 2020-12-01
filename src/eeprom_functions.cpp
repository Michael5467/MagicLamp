#include "eeprom_function.h"


uint8_t eeprom_read_1B (uint16_t address)
{
    uint8_t data;
    uint8_t* p = (uint8_t *)&data;
    *p       = EEPROM.read(address);
    return data;
}

uint16_t eeprom_read_2B(uint16_t address)
{
    uint16_t data;
    uint8_t* p = (uint8_t *)&data;
    *p       = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    return data;
}

uint32_t eeprom_read_4B(uint16_t address)
{
    uint32_t data;
    uint8_t* p = (uint8_t*)&data;
    *p        = EEPROM.read(address);
    *(p + 1)  = EEPROM.read(address + 1);
    *(p + 2)  = EEPROM.read(address + 2);
    *(p + 3)  = EEPROM.read(address + 3);
    return data;
}

bool eeprom_write_1B(uint16_t address, int8_t data)
{
    uint8_t* p = (uint8_t*)&data;
    EEPROM.write(address, *p);
    return EEPROM.commit();      
}

bool eeprom_write_2B(uint16_t address, int16_t data)
{
    uint8_t* p = (uint8_t*)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    return EEPROM.commit();      
}

bool eeprom_write_4B(uint16_t address, int32_t data)
{
    uint8_t* p = (uint8_t*)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    EEPROM.write(address + 2, *(p + 2));
    EEPROM.write(address + 3, *(p + 3));
    return EEPROM.commit();      
}
