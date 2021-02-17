#include "configuration_functions.h"

File cfg_file;

void emptyConfig(the_lamp_state_t &lamp_state, lamp_config_s &lamp_config)
{
    DPRINTLNF("Default config...");
    lamp_config.version.major = VERSION_MAJOR;
    lamp_config.version.minor = VERSION_MINOR;
    lamp_config.name[31] = 0;   // Hard 'end of string' for string function safety.
    String defaultName = F("MagicLamp");
    strcpy(lamp_config.name, defaultName.c_str());
    lamp_config.state = true;
    lamp_config.effect = getEffectFromLampState(lamp_state);
    for (uint8_t i=0; i < 7; i++)
    {
        lamp_config.alarm[i].state = alarm_state_t::alarm_off;
        lamp_config.alarm[i].hour  = 7;
        lamp_config.alarm[i].min   = 0;
    }
}

void printRawConfig(lamp_config_s *config)
{
    uint8_t *p = (uint8_t *)config;

    DPRINTLNF("\nprintRawConfig()");
    for (uint8_t i=0; i < sizeof(lamp_config_s); i++)
    {
        DPRINT(*(p+i));
        DPRINTF(", ");
    }
    DPRINTLNF("\n");
}

void printConfig(lamp_config_s &config, boolean printAlarm)
{
    DPRINTLNF("\nprintConfig()");
    DPRINTF("Version = ");
    DPRINT(config.version.major);
    DPRINTF(".");
    DPRINTLN(config.version.minor);
    // DPRINTF("Name = {");
    // // DPRINT(config.name);
    // String tmpStr = config.name;
    // DPRINT(tmpStr.c_str());
    // DPRINTLNF("}");
    printString(config.name);
    DPRINTLN_FULL(config.state);
    DPRINTLN_FULL(config.effect.number);
    DPRINTLN_FULL(config.effect.brightness);
    DPRINTLN_FULL(config.effect.speed);
    DPRINTLN_FULL(config.effect.scale);

    if (printAlarm)
    {
        for (uint8_t i=0; i < 7; i++)
        {
            DPRINTLN_FULL(config.alarm[i].state);
            DPRINTLN_FULL(config.alarm[i].hour);
            DPRINTLN_FULL(config.alarm[i].min);
        }
    }

    DPRINTLN_FULL(config.dirty);
}

void readConfig(lamp_config_s &config)
{
    config.version = readVersion();
    readHostName(config.name, 32);
    config.state = readState();
    config.effect = readEffect();
    for (uint8_t i=0; i < 7; i++)
    {
        config.alarm[i] = readAlarm(i);
    }
}

void writeConfig(lamp_config_s &config)
{
    writeVersion(config.version);
    writeHostName(config.name);
    writeState(config.state);
    writeEffect(config.effect);
    for (uint8_t i=0; i < 7; i++)
    {
        writeAlarm(config.alarm[i], i);
    }
}

void readRawConfig(lamp_config_s *config)
{
    openConfiguration("r");
    cfg_file.read((uint8_t *)config, sizeof(lamp_config_s));
    closeConfiguration();
}

void writeRawConfig(lamp_config_s *config)
{
    openConfiguration("w");
    cfg_file.seek(EEPROM_ADDRESSES::BASE, fs::SeekSet);
    cfg_file.write((uint8_t *)config, sizeof(lamp_config_s));
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

void openConfiguration(const char* mode)
{
    DPRINTLNF("openConfiguration():");
    if (!LittleFS.exists(CONFIG_FILE_NAME))
    {
        DPRINTLNF("CONFIG_FILE_NAME is not exist");
        cfg_file = LittleFS.open(CONFIG_FILE_NAME, "w");
        for (uint8_t i=0; i < sizeof(lamp_config_s); i++)
        {
            cfg_file.write(0);
        }
        cfg_file.close();
        DPRINTLNF("CONFIG_FILE_NAME is sreated");
    }
    else
    {
        DPRINTLNF("CONFIG_FILE_NAME is exist");
    }
    cfg_file = LittleFS.open(CONFIG_FILE_NAME, mode);
    DPRINTLNF("CONFIG_FILE_NAME is opened");
}

void saveConfiguration()
{
    cfg_file.flush();
}

void closeConfiguration()
{
    cfg_file.close();
}

version_t readVersion()
{
    version_t version;

    openConfiguration("r");
    cfg_file.seek(EEPROM_ADDRESSES::version, fs::SeekSet);
    cfg_file.read((uint8_t *) &version, 2);
    closeConfiguration();

    return version;
}

void readHostName(char *str, uint8_t max_size = 32)
{
    openConfiguration("r");
    cfg_file.seek(EEPROM_ADDRESSES::name, fs::SeekSet);
    cfg_file.read((uint8_t *) str, max_size);
    closeConfiguration();
}

boolean readState()
{
    openConfiguration("r");
    cfg_file.seek(EEPROM_ADDRESSES::state, fs::SeekSet);
    closeConfiguration();

    return (boolean)cfg_file.read();
}

effect_s readEffect()
{
    effect_s effect;

    openConfiguration("r");
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_number);
    cfg_file.read((uint8_t *) &effect.number, 1);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_brightness);
    cfg_file.read((uint8_t *) &effect.brightness, 1);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_speed);
    cfg_file.read((uint8_t *) &effect.speed, 4);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_scale);
    cfg_file.read((uint8_t *) &effect.scale, 2);
    closeConfiguration();

    return effect;
}

alarm_s readAlarm(uint8_t day)
{

    uint16_t day_base = (EEPROM_ADDRESSES::alarms) + day * (EEPROM_ADDRESSES::ALARM_SIZE);
    alarm_s alarm;

    openConfiguration("r");
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_state);
    cfg_file.read((uint8_t *) &alarm.state, 1);
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_hour);
    cfg_file.read((uint8_t *) &alarm.hour, 1);
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_min);
    cfg_file.read((uint8_t *) &alarm.min, 1);
    closeConfiguration();

    return alarm;
}

void writeVersion(version_t &version)
{
    openConfiguration("w");
    cfg_file.seek(EEPROM_ADDRESSES::version, fs::SeekSet);
    cfg_file.write((uint8_t *) &version, 2);
    closeConfiguration();
}

void writeHostName(const char *str)
{
    openConfiguration("w");
    cfg_file.seek(EEPROM_ADDRESSES::name, fs::SeekSet);
    cfg_file.write((uint8_t *) str, 32);
    closeConfiguration();
}

void writeState(boolean state)
{
    openConfiguration("w");
    cfg_file.seek(EEPROM_ADDRESSES::state, fs::SeekSet);
    cfg_file.write((uint8_t)state);
    closeConfiguration();
}

void writeEffect(effect_s &effect)
{
    openConfiguration("w");
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_number);
    cfg_file.write((uint8_t *) &effect.number, 1);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_brightness);
    cfg_file.write((uint8_t *) &effect.brightness, 1);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_speed);
    cfg_file.write((uint8_t *) &effect.speed, 4);
    cfg_file.seek(EEPROM_ADDRESSES::effect + EEPROM_ADDRESSES::effect_scale);
    cfg_file.write((uint8_t *) &effect.scale, 2);
    closeConfiguration();
}

void writeAlarm(alarm_s &alarm, uint8_t day)
{
    uint16_t day_base = (EEPROM_ADDRESSES::alarms) + day * (EEPROM_ADDRESSES::ALARM_SIZE);

    openConfiguration("w");
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_state);
    cfg_file.write((uint8_t)alarm.state);
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_hour);
    cfg_file.write(alarm.hour);
    cfg_file.seek(day_base + EEPROM_ADDRESSES::alarm_min);
    cfg_file.write(alarm.min);
    closeConfiguration();
}
