#ifndef __INC_PROJECT_CONFIG_H
#define __INC_PROJECT_CONFIG_H

#include "board_config.h"
#include "effects.h"
#include "ntp_config.h"
#include "Michael_MinimalTimer.h"

#define DEBUG_PRINT
#define DEBUG_PRINT_SNAKE

// ESP32 config:
// 0 - derect WIFI access
// 1 - access point
#define ESP_MODE 0

// Parameters for direct WIFI connecting
// #define DA_SSID "AndroidAP_7811"
// #define DA_PASS "94d61111"

#define DA_SSID "iPhone (Elena)"
#define DA_PASS "ax09280588"

// #define DA_SSID "DL-F2-18"
// #define DA_PASS "mgtswifi"

// Access point parameters
#define AP_SSID "AutoConnectAP"
#define AP_PASS "12345678"

// WEB port (80 as default)
#define HTTP_PORT 80

// Preset for FastLED library
// #define FASTLED_INTERRUPT_RETRY_COUNT 0
// #define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
// #define FASTLED_ESP8266_NODEMCU_PIN_ORDER
// #define FASTLED_ESP8266_D1_PIN_ORDER

// Interval between "idle" print and WDT timer updates (in seconds)
#define IDLE_TIME 60

// Interval between dawn-time checking
#define DAWN_CHECK_TIME 60

// Total amount of modes
#define MODES_AMOUNT 17

// Effects configurations: delays, timeout, speed...
#define START_EFFECT EFF_CODE_SNOW // start effect
#define EFFECT_SPEED 180           // default effects speed (ms)

#define USE_NOISE_EFFECTS 1        // fullscreen effects are uses

// Games
#define USE_SNAKE 1

//#define USE_PARAMS_CHECK

#endif // __INC_PROJECT_CONFIG_H