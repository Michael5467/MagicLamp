#ifndef __INC_EEPROM_FUNCTIONS_H
#define __INC_EEPROM_FUNCTIONS_H 

#include <Arduino.h>
#include <EEPROM.h>

uint8_t  eeprom_read_1B(uint16_t address);
uint16_t eeprom_read_2B(uint16_t address);
uint32_t eeprom_read_4B(uint16_t address);

bool eeprom_write_1B(uint16_t address, uint8_t data);
bool eeprom_write_2B(uint16_t address, uint16_t data);
bool eeprom_write_4B(uint16_t address, uint32_t data);

String eeprom_read_string(uint16_t startAdress, uint16_t endAdress);

#endif // __INC_EEPROM_FUNCTIONS_H