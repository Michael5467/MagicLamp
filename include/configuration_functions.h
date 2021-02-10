#ifndef __INC_CONFIGURATION_FUNCTIONS_H
#define __INC_CONFIGURATION_FUNCTIONS_H 

#include <Arduino.h>
#include <LittleFS.h>

#include "functions.h"
#include "eeprom_function.h"

struct EEPROM_ADDRESSES
{
    enum ADDRESS_MAP : uint16_t
    {
        version = 0x0,
        name    = 0x2,
        state   = 0x22,
        effect  = 0x23,
        alarms  = 0x2B,
        BASE    = 0x0       // Base address of the lamp's state structure in EEPROM (equal first variable 'version')
    };

    enum EFFECT_OFFSETS : uint16_t
    {
        effect_number      = 0x0,
        effect_brightness  = 0x1,
        effect_speed       = 0x2,
        effect_scale       = 0x6,
        EFFECT_SIZE        = 0x8
    };

    enum ALARM_OFFSETS : uint16_t
    {
        alarm_state = 0x0,
        alarm_hour,
        alarm_min,
        ALARM_SIZE
    };
};

void emptyConfig(the_lamp_state_t &lamp_state, lamp_config_s &lamp_config);
effect_s getEffectFromLampState(the_lamp_state_t &lamp_state);
void setEffectToLampState(the_lamp_state_t &lamp_state, effect_s &effect);

void openConfiguration();
void saveConfiguration();
void closeConfiguration();

version_t readVersion();
void      readHostName(char *str, uint8_t max_size);
boolean   readState();
effect_s  readEffect();
alarm_s   readAlarm(uint8_t day);

void writeVersion(version_t &version);
void writeHostName(const char *str);
void writeState(boolean state);
void writeEffect(effect_s &effect);
void writeAlarm(alarm_s &alarm, uint8_t day);

void readConfig(lamp_config_s &config);
void writeConfig(lamp_config_s &config);
void readRawConfig(lamp_config_s *config);
void writeRawConfig(lamp_config_s *config);

void printConfig(lamp_config_s &config);
void printRawConfig(lamp_config_s *config);

#endif // __INC_CONFIGURATION_FUNCTIONS_H
