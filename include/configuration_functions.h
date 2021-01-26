#ifndef __INC_CONFIGURATION_FUNCTIONS_H
#define __INC_CONFIGURATION_FUNCTIONS_H 

#include <Arduino.h>
#include "functions.h"
#include "eeprom_function.h"

enum class EEPROM_ADDRESSES: uint16_t
{
    version = 0x0,
    name    = 0x2,
    state   = 0xB,
    effect  = 0xC,
    alarms  = 0x14
};

enum class EEPROM_OFFSETS: uint16_t
{
    alarm = 0x3
};

void openConfiguration();
void saveConfiguration();
void closeConfiguration();

version_t readVersion();
String    readHostName();
boolean   readState();
effect_t  readEffect();
alarm_t   readAlarm(uint8_t day);

void writeVersion(version_t &version);
void writeHostName(String &hostname);
void writeState(boolean state);
void writeEffect(effect_t &effect);
void writeAlarm(alarm_t &alarm, uint8_t day);

#endif // __INC_CONFIGURATION_FUNCTIONS_H
