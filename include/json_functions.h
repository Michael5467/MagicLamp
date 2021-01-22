#ifndef __INC_JSON_FUNCTIONS_H
#define __INC_JSON_FUNCTIONS_H

#include <Arduino.h>
#include <ArduinoJson.h>
#include <LittleFS.h>

#include "functions.h"
#include "debug_macros.h"

extern alarm_t awake_alarm_arr[];
extern alarm_t sleep_alarm;

void loadConfiguration(const char *filename, the_lamp_state_t &config);
void saveConfiguration(const char *filename, const the_lamp_state_t &config);
void printConfiguration(const char *filename);

void debugPrintVariant(const JsonVariant& object, const char* key);

#endif // __INC_JSON_FUNCTIONS_H