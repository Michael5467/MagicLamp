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
the_lamp_state_t save_lamp_state;
uint32_t save_Effect_Timer_Interval;

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

    // Alarm Test
    for (int i = 0; i <= 6; i++) {
        awake_alarm_arr[i].state = true;
        awake_alarm_arr[i].day = 3;
        awake_alarm_arr[i].hour = 15;
        awake_alarm_arr[i].min = 45;
    }
}

void loop() {
    ServerLoop(&server, &lamp_state);

    // Working with matrix
#ifdef DEBUG_STEP
    if (lamp_state.debug) {
        if (lamp_state.step) {
            SelectEffect(&lamp_state);  // Current effect drawing
            FastLED.show();             // Show matrix
            lamp_state.step = false;
        }
    }
    else {
        if (lamp_state.state && Effect_Timer.isReady()) {
            SelectEffect(&lamp_state);  // Current effect drawing
            FastLED.show();             // Show matrix
        }
    }
#else
    if (lamp_state.state && Effect_Timer.isReady()) {
        SelectEffect(&lamp_state);  // Current effect drawing
        FastLED.show();             // Show matrix
    }
#endif

    // Dawn check
    if (Clock_Timer.isReady()) {
        time_t curr_Time = lamp_state.date_time->local_time + (Idle_Timer.getMillis()-lamp_state.date_time->local_millis)/1000;

        uint8_t thisDay = convert_to_ISO8601(dayOfWeek(curr_Time));
        if (awake_alarm_arr[thisDay].state) {
            int hour_var = hour(curr_Time);
            int min_var  = minute(curr_Time);
            if ((awake_alarm_arr[thisDay].hour == hour_var) and (awake_alarm_arr[thisDay].min == min_var)) {
                save_lamp_state = lamp_state;
                save_Effect_Timer_Interval = lamp_state.effectTimer->getInterval();
                setEffectsSpeed(&lamp_state, EFFECT_SPEED);
                lamp_state.state = true;
                lamp_state.dawn = true;
                setMode(&lamp_state, EFF_DAWN);
            }
        }
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
        DPRINTLN("\nidleTimer.isReady()");
        printTime(lamp_state.date_time->local_time + (Idle_Timer.getMillis()-lamp_state.date_time->local_millis)/1000);

        // uint8_t thisDay = dayOfWeek(lamp_state.date_time->local_time + (Idle_Timer.getMillis()-lamp_state.date_time->local_millis)/1000);
        // if (thisDay == 1)
        //     thisDay = 8;
        // thisDay -= 2;
        // DPRINTLN_FULL(thisDay);
        // // uint8_t d = day(lamp_state.date_time->local_time + (Idle_Timer.getMillis()-lamp_state.date_time->local_millis)/1000);
        ESP.wdtFeed();
    }
}
