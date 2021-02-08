#include "eeprom_function.h"

void eeprom_start(uint16_t size)
{
    EEPROM.begin(size);
}

void eeprom_commit()
{
    EEPROM.commit();
}

void eeprom_end()
{
    EEPROM.end();
}

uint8_t eeprom_read_1B(uint16_t address)
{
    uint8_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    return data;
}

uint16_t eeprom_read_2B(uint16_t address)
{
    uint16_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    return data;
}

uint32_t eeprom_read_4B(uint16_t address)
{
    uint32_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    *(p + 2) = EEPROM.read(address + 2);
    *(p + 3) = EEPROM.read(address + 3);
    return data;
}

void eeprom_write_1B(uint16_t address, uint8_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
}

void eeprom_write_2B(uint16_t address, uint16_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
}

void eeprom_write_4B(uint16_t address, uint32_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    EEPROM.write(address + 2, *(p + 2));
    EEPROM.write(address + 3, *(p + 3));
}

void eeprom_read_string(uint16_t address, char *str, uint8_t max_size = 32)
{
    uint16_t offset = 0;
    char c = 0;

    do
    {
        c = char(EEPROM.read(address + offset));
        *(str + offset) = c;
        offset++;
    } while (c && (offset < max_size));    
}

void eeprom_write_string(uint16_t address, const char *str, uint8_t max_size = 32)
{
    uint8_t byte;
    uint16_t offset = 0;

    do
    {
        byte = (uint8_t) *(str + offset);
        EEPROM.write(address + offset, byte);
        offset++;
    } while (byte && (offset < max_size));    
}
