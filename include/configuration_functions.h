#ifndef __INC_CONFIGURATION_FUNCTIONS_H
#define __INC_CONFIGURATION_FUNCTIONS_H 

#include <Arduino.h>
#include "functions.h"
#include "eeprom_function.h"

class EEPROM_ADDRESSES
{
private:
public:
    static const uint16_t version = 0x0;
    static const uint16_t name    = 0x2;
    static const uint16_t state   = 0xB;
    static const uint16_t effect  = 0xC;
    static const uint16_t alarms  = 0x14;

    enum EFFECT_OFFSETS: uint16_t
    {
        effect_number      = 0x0,
        effect_brightness  = 0x1,
        effect_speed       = 0x2,
        effect_scale       = 0x6,
        EFFECT_SIZE        = 0x8
    };

    enum ALARM_OFFSETS: uint16_t
    {
        alarm_state = 0x0,
        alarm_hour,
        alarm_min,
        ALARM_SIZE
    };
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
