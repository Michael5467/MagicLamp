#include "configuration_functions.h"

void printConfig(lamp_config_s &config)
{
    DPRINTF("Version = ");
    DPRINT(config.version.major);
    DPRINTF(".");
    DPRINTLN(config.version.minor);
    DPRINTF("Name = {");
    DPRINT(config.name);
    DPRINTLNF("}");
    DPRINTLN_FULL(config.state);
    DPRINTLN_FULL(config.effect.number);
    DPRINTLN_FULL(config.effect.brightness);
    DPRINTLN_FULL(config.effect.speed);
    DPRINTLN_FULL(config.effect.scale);
    for (uint8_t i=0; i < 7; i++)
    {
        DPRINTLN_FULL(config.alarm[i].state);
        DPRINTLN_FULL(config.alarm[i].hour);
        DPRINTLN_FULL(config.alarm[i].min);
    }
}

lamp_config_s readConfig()
{
    lamp_config_s config;

    openConfiguration();
    config.version = readVersion();

    // String constName = "MagicLamp";
    String constName = readHostName();
    strcpy(config.name, constName.c_str());
    // memcpy (config.name, cstr, 9);

    config.state = readState();
    config.effect = readEffect();
    for (uint8_t i=0; i < 7; i++)
    {
        config.alarm[i] = readAlarm(i);
    }

    closeConfiguration();
    return config;
}

void writeConfig(lamp_config_s &config)
{
    openConfiguration();
    writeVersion(config.version);
    writeHostName(config.name);
    writeState(config.state);
    writeEffect(config.effect);
    for (uint8_t i=0; i < 7; i++)
    {
        writeAlarm(config.alarm[i], i);
    }
    saveConfiguration();
    closeConfiguration();
}

effect_s getEffectFromLampState(the_lamp_state_t &lamp_state)
{
    effect_s effect;

    effect.number     = lamp_state.effect;
    effect.brightness = lamp_state.brightness;
    effect.speed      = lamp_state.speed;
    effect.scale      = lamp_state.scale;
    return effect;
}

void setEffectToLampState(the_lamp_state_t &lamp_state, effect_s &effect)
{
    lamp_state.effect     = effect.number;
    lamp_state.brightness = effect.brightness;
    lamp_state.speed      = effect.speed;
    lamp_state.scale      = effect.scale;
    convertRAW(&lamp_state);
}

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

    version.version = eeprom_read_2B((EEPROM_ADDRESSES::version));
    return version;
}

String readHostName()
{
    String hostname = "";

    hostname = eeprom_read_string(EEPROM_ADDRESSES::name, 32);
    return hostname;
}

boolean readState()
{
    return (boolean)eeprom_read_1B(EEPROM_ADDRESSES::state);
}

effect_s readEffect()
{
    effect_s effect;

    effect.number     = eeprom_read_1B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_number);
    effect.brightness = eeprom_read_1B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_brightness);
    effect.speed      = eeprom_read_4B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_speed);
    effect.scale      = eeprom_read_2B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_scale);
    return effect;
}

alarm_s readAlarm(uint8_t day)
{
    uint16_t day_base = (EEPROM_ADDRESSES::alarms) + day * (EEPROM_ADDRESSES::ALARM_SIZE);
    alarm_s alarm;

    alarm.state = (alarm_state_t)eeprom_read_1B(day_base + EEPROM_ADDRESSES::alarm_state);
    alarm.hour  =                eeprom_read_1B(day_base + EEPROM_ADDRESSES::alarm_hour);
    alarm.min   =                eeprom_read_1B(day_base + EEPROM_ADDRESSES::alarm_min);
    return alarm;
}

void writeVersion(version_t &version)
{
    eeprom_write_2B((EEPROM_ADDRESSES::version), version.version);
}

void writeHostName(const char *hostname)
{
    eeprom_write_string(EEPROM_ADDRESSES::name, hostname, 32);
}

void writeState(boolean state)
{
    eeprom_write_1B((EEPROM_ADDRESSES::state), (uint8_t)state);
}

void writeEffect(effect_s &effect)
{
    eeprom_write_1B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_number,     effect.number);
    eeprom_write_1B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_brightness, effect.brightness);
    eeprom_write_4B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_speed,      effect.speed);
    eeprom_write_2B(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_scale,      effect.scale);
}

void writeAlarm(alarm_s &alarm, uint8_t day)
{
    uint16_t day_base = (EEPROM_ADDRESSES::alarms) + day * (EEPROM_ADDRESSES::ALARM_SIZE);

    eeprom_write_1B(day_base + (EEPROM_ADDRESSES::alarm_state), alarm.state);
    eeprom_write_1B(day_base + (EEPROM_ADDRESSES::alarm_hour),  alarm.hour);
    eeprom_write_1B(day_base + (EEPROM_ADDRESSES::alarm_min),   alarm.min);
}
