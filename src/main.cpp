#include <Arduino.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <WiFiUdp.h>

#include "project_config.h"
#include "functions.h"
#include "effects.h"
#include "matrix.h"

snow_parameters_t effect_snow = {SNOW_DENSE, SNOW_COLOR, SNOW_COLOR_STEP};
the_lamp_state_t lamp_state;

#include "FastLED.h"
CRGB leds[NUM_LEDS];

const char *autoConnectSSID = DA_SSID;
const char *autoConnectPass = DA_PASS;

const char *accesspointSSID = AP_SSID;
const char *accesspointPass = AP_PASS;

// Set web server port number
WiFiServer server(HTTP_PORT);

boolean loadingFlag = true; // TODO: global variable, remove to local...

#include "Michael_MinimalTimer.h"
M_MinimalTimer effectTimer(EFFECT_SPEED);
#ifdef DEBUG_PRINT
M_MinimalTimer idleTimer((long)IDLE_TIME * 10000);
#endif

// void init() {
//     effect_snow.dencity = SNOW_DENSE;
//     effect_snow.color   = 
// }

void setup() {
#ifdef DEBUG_PRINT
    Serial.begin(74880);
    Serial.println("NodeMCU v3...............................");
#endif

    // Initial lamp state
    lamp_state.state        = true;
    lamp_state.effect       = START_EFFECT;
    lamp_state.brightness   = BRIGHTNESS;
    lamp_state.speed        = EFFECT_SPEED;
    lamp_state.scale        = 5;
    lamp_state.IP           = "";
    lamp_state.loadingFlag  = false;
    lamp_state.effectTimer  = &effectTimer;
    lamp_state.leds         = leds;
    lamp_state.effect_snow  = &effect_snow;

    // LED matrix: strip configuration and instantiation
    FastLED.addLeds<WS2812, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness(lamp_state.brightness);
    if (CURRENT_LIMIT > 0) 
        FastLED.setMaxPowerInVoltsAndMilliamps(5, CURRENT_LIMIT);
    FastLED.clear();
    FastLED.show();

    randomSeed(micros());

    // WIFI
    if (ESP_MODE == 0) {
#ifdef DEBUG_PRINT
        Serial.print("Connecting to ");
        Serial.println(autoConnectSSID);
#endif
        WiFi.begin(autoConnectSSID, autoConnectPass);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
#ifdef DEBUG_PRINT
            Serial.print(".");
#endif
        }
    }
    else {    
#ifdef DEBUG_PRINT
        Serial.print("WiFi manager");
#endif
        WiFiManager wifiManager;
        wifiManager.setDebugOutput(false);
#if (USE_BUTTON == 1)
        if (digitalRead(BTN_PIN))
            wifiManager.resetSettings();
#endif
        wifiManager.autoConnect(accesspointSSID, accesspointPass);
    }
#ifdef DEBUG_PRINT
    Serial.print("Connected! IP address: ");
    Serial.println(WiFi.localIP());
#endif
    lamp_state.IP = WiFi.localIP().toString();

    // HTTP server
    server.begin();
}

void loop() {
    ServerLoop(&server, &lamp_state);

    // Working with matrix
    if (lamp_state.state && effectTimer.isReady()) {
        SelectEffect(&lamp_state);  // Current effect drawing
        FastLED.show();             // Show matrix
    }

    // Idle timer: for debug only
    if (idleTimer.isReady()) {      
#ifdef DEBUG_PRINT
        Serial.println("idleTimer.isReady()");
#endif
        ESP.wdtFeed();
        // yield();  // ещё раз пнуть собаку
    }
}