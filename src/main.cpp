#include <Arduino.h>

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager

#include <WiFiUdp.h>

#include <NTPClient.h>
#include <Timezone.h>

#include <FastLED.h>

#include "project_config.h"
#include "functions.h"
#include "effects.h"
#include "matrix.h"
#include "Michael_MinimalTimer.h"

snow_parameters_t effect_snow = {SNOW_DENSE, SNOW_COLOR, SNOW_COLOR_STEP};
local_date_time_t date_time;
the_lamp_state_t lamp_state;

CRGB leds[NUM_LEDS];

const char *autoConnectSSID = DA_SSID;
const char *autoConnectPass = DA_PASS;

const char *accesspointSSID = AP_SSID;
const char *accesspointPass = AP_PASS;

// Set web server port number
WiFiServer server(HTTP_PORT);

boolean loadingFlag = true; // TODO: global variable, remove to local...

M_MinimalTimer Effect_Timer(EFFECT_SPEED);
M_MinimalTimer Clock_Timer(CLOCK_TIME * 1000);
M_MinimalTimer Idle_Timer(IDLE_TIME * 1000);
// M_MinimalTimer Dawn_Timer(DAWN_CHECK_TIME * 1000);
M_MinimalTimer NTP_Timer(NTP_INITIAL_INTERVAL);

WiFiManager wifiManager;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, NTP_ADDRESS, 0, NTP_INTERVAL);

alarm_t awake_alarm_arr[7];
alarm_t sleep_alarm;

void setup() {
    // Serial port setup
    Serial.begin(74880);
    DPRINTLN("NodeMCU v3...............................");

    // Initial lamp state
    lamp_state.state        = true;
    lamp_state.effect       = START_EFFECT;
    lamp_state.brightness   = BRIGHTNESS;
    lamp_state.speed        = EFFECT_SPEED;
    lamp_state.scale        = 5;
    lamp_state.IP           = "";
    lamp_state.loadingFlag  = false;
    lamp_state.date_time    = &date_time;
    lamp_state.effectTimer  = &Effect_Timer;
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
        DPRINT("Connecting to ");
        DPRINTLN(autoConnectSSID);
        WiFi.begin(autoConnectSSID, autoConnectPass);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            DPRINT(".");
        }
    }
    else {    
        DPRINT("WiFi manager ");
        wifiManager.setDebugOutput(false);
#if (USE_BUTTON == 1)
        if (digitalRead(BTN_PIN))
            wifiManager.resetSettings();
#endif
		wifiManager.autoConnect(accesspointSSID, accesspointPass);
    }
    DPRINT("Connected! IP address: ");
    DPRINTLN(WiFi.localIP());
    lamp_state.IP = WiFi.localIP().toString();

    // HTTP server
    server.begin();

    // NTP client
    timeClient.begin();

    // WDT
    ESP.wdtFeed();
    updateMode(&lamp_state);
}

void loop() {
    ServerLoop(&server, &lamp_state);

    // Working with matrix
    if (lamp_state.state && Effect_Timer.isReady()) {
        SelectEffect(&lamp_state);  // Current effect drawing
        FastLED.show();             // Show matrix
    }

    // Dawn check
    if (Clock_Timer.isReady()) {
        uint32_t get_Millis;

        get_Millis = Clock_Timer.getMillis();
        DPRINTLN_FULL(get_Millis);
        // lamp_state.date_time->local_time += (Clock_Timer.getMillis()-lamp_state.date_time->local_millis)/1000;
        lamp_state.date_time->local_time += (Clock_Timer.getMillis()-lamp_state.date_time->local_millis)/10000;
    }

    // NTP connection and date/time update
    if (NTP_Timer.isReady()) {      
        if (NTP_Synchronization(&timeClient, lamp_state.date_time)) {
            NTP_Timer.setInterval(NTP_INTERVAL);
        } else {
            NTP_Timer.setInterval(NTP_INITIAL_INTERVAL);
        }
        DPRINT("NTP_Timer _interval = ");
        DPRINTLN(NTP_Timer.getInterval());
    }

    // Idle timer: for WDT and debug
    if (Idle_Timer.isReady()) {      
        DPRINTLN("idleTimer.isReady()");
        printTime(lamp_state.date_time->local_time);
        ESP.wdtFeed();
    }
}
