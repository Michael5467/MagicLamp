#include "configuration_functions.h"

boolean configReady = false;

void openConfiguration()
{
    eeprom_start(64);
}

void saveConfiguration()
{
    eeprom_commit();
}

void closeConfiguration()
{
    eeprom_end();
}

version_t readVersion()
{
    version_t version;
    version.version = eeprom_read_2B(static_cast<uint16_t>(EEPROM_ADDRESSES::version));
    return version;
}

String readHostName()
{
    String hostname = "";
    hostname = eeprom_read_string(static_cast<uint16_t>(EEPROM_ADDRESSES::name));
    return hostname;
}

boolean readState()
{
    return (boolean)eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::state));
}

effect_t readEffect()
{
    effect_t effect;
    effect.number     = eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect)    );
    effect.brightness = eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 1);
    effect.speed      = eeprom_read_4B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 2);
    effect.scale      = eeprom_read_2B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 6);
    return effect;
}

alarm_t readAlarm(uint8_t day)
{
    alarm_t alarm;
    alarm.state = static_cast<alarm_state_t>(eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day)   );
    alarm.hour  =                            eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day + 1);
    alarm.min   =                            eeprom_read_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day + 2);
    return alarm;
}

void writeVersion(version_t &version)
{
    eeprom_write_2B(static_cast<uint16_t>(EEPROM_ADDRESSES::version), version.version);
    eeprom_commit();
}

void writeHostName(String &hostname)
{
    return;
}

void writeState(boolean state)
{
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::state), (uint8_t)state);
    eeprom_commit();
}

void writeEffect(effect_t &effect)
{
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect),     effect.number);
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 1, effect.brightness);
    eeprom_write_4B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 2, effect.speed);
    eeprom_write_2B(static_cast<uint16_t>(EEPROM_ADDRESSES::effect) + 6, effect.scale);
    eeprom_commit();
}

void writeAlarm(alarm_t alarm, uint8_t day)
{
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day,     static_cast<uint8_t>(alarm.state));
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day + 1, alarm.hour);
    eeprom_write_1B(static_cast<uint16_t>(EEPROM_ADDRESSES::alarms) + 3 * day + 2, alarm.min);
    eeprom_commit();
}
