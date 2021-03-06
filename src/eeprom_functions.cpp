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
    DPRINTF("eeprom_read_1B(): ");
    DPRINTLN_FULL(data);
    return data;
}

uint16_t eeprom_read_2B(uint16_t address)
{
    uint16_t data;
    uint8_t *p = (uint8_t *)&data;
    *p = EEPROM.read(address);
    *(p + 1) = EEPROM.read(address + 1);
    DPRINTF("eeprom_read_2B(): ");
    DPRINTLN_FULL(data);
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
    DPRINTF("eeprom_read_4B(): ");
    DPRINTLN_FULL(data);
    return data;
}

void eeprom_write_1B(uint16_t address, uint8_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    DPRINTF("eeprom_write_1B(): *p = ");
    DPRINTLN(*p);
}

void eeprom_write_2B(uint16_t address, uint16_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    DPRINTF("eeprom_write_2B(): *p = ");
    DPRINT_FULL(*p);
    DPRINTLN_FULL(*(p+1));
}

void eeprom_write_4B(uint16_t address, uint32_t data)
{
    uint8_t *p = (uint8_t *)&data;
    EEPROM.write(address, *p);
    EEPROM.write(address + 1, *(p + 1));
    EEPROM.write(address + 2, *(p + 2));
    EEPROM.write(address + 3, *(p + 3));
    DPRINTF("eeprom_write_2B(): ");
    DPRINT_FULL(*p);
    DPRINT_FULL(*(p+1));
    DPRINT_FULL(*(p+2));
    DPRINTLN_FULL(*(p+3));
}

void eeprom_read_string(uint16_t address, char *str, uint8_t max_size = 32)
{
    uint16_t offset = 0;
    char c = 0;

    DPRINTF("eeprom_read_string(): ");
    do
    {
        c = char(EEPROM.read(address + offset));
        if (c)
        {
            DPRINT(c);
            DPRINTF(", ")
        }
        *(str + offset) += c;
        offset++;
    } while (c && (offset < max_size));    
    DPRINTLNF(" ");
}

void eeprom_write_string(uint16_t address, const char *str, uint8_t max_size = 32)
{
    uint8_t byte;
    uint16_t offset = 0;

    DPRINTF("eeprom_write_string(): ");
    do
    {
        byte = (uint8_t) *(str + offset);
        DPRINT(byte);
        DPRINTF(", ")
        EEPROM.write(address + offset, byte);
        offset++;
    } while (byte && (offset < max_size));    
    DPRINTLNF(" ");
}
