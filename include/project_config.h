#ifndef __INC_PROJECT_CONFIG_H
#define __INC_PROJECT_CONFIG_H

#include "board_config.h"
#include "effects.h"
#include "ntp_config.h"
#include "Michael_MinimalTimer.h"

#include "debug_macros.h"

#ifndef VERSION_MAJOR
	#define VERSION_MAJOR  0
#endif

#ifndef VERSION_MINOR
	#define VERSION_MINOR  0
#endif

// ESP32 config:
// 0 - derect WIFI access
// 1 - access point
#define ESP_MODE 0

// Parameters for direct WIFI connecting
// #define DA_SSID "AndroidAP_6955"
// #define DA_PASS "12345666"

// #define DA_SSID "iPhone (Elena)"
// #define DA_PASS "ax09280588"

#define DA_SSID "DL-F2-18"
#define DA_PASS "mgtswifi"

// Access point parameters
#define AP_SSID "AutoConnectAP"
#define AP_PASS "12345678"

// WEB port (80 as default)
#define LAMP_HTTP_PORT 80
// WEB SOKET port (81 as default)
#define LAMP_WEB_SOKET_PORT 81

#define CONFIG_FILE_NAME F("config.bin")

// Preset for FastLED library
// #define FASTLED_INTERRUPT_RETRY_COUNT 0
// #define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_ESP8266_RAW_PIN_ORDER
// #define FASTLED_ESP8266_NODEMCU_PIN_ORDER
// #define FASTLED_ESP8266_D1_PIN_ORDER

// Timers' delay parameters (in seconds)
#define IDLE_TIME  15 // 60    // Interval between "idle" print and WDT timer updates
#define CLOCK_TIME 10    // Interval between clock updates (and dawn checking)
// #define DAWN_CHECK_TIME 10 // Interval between dawn-time checking

// Total amount of modes
#define MODES_AMOUNT 17

// Effects configurations: delays, timeout, speed...
#define START_EFFECT EFF_CODE_SNAKE // EFF_CODE_SNOW // start effect
#define EFFECT_SPEED 180           // default effects speed (ms)

#define USE_NOISE_EFFECTS 1        // fullscreen effects are used

// Games
#define USE_SNAKE 1

//#define USE_PARAMS_CHECK

#endif // __INC_PROJECT_CONFIG_H