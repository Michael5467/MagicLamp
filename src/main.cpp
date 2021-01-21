#include <Arduino.h>

// #define DEBUG_ESP_HTTP_SERVER
#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino

//needed for library
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>
#include <WiFiManager.h>         //https://github.com/tzapu/WiFiManager
#include <ESP8266SSDP.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <NTPClient.h>
#include <Timezone.h>

#include <FastLED.h>

#include "project_config.h"
#include "functions.h"
#include "web_functions.h"
#include "effects.h"
#include "matrix.h"
#include "json_functions.h"
#include "Michael_MinimalTimer.h"

local_date_time_t date_time;
the_lamp_state_t lamp_state;

CRGB leds[NUM_LEDS];

const char *autoConnectSSID = DA_SSID;
const char *autoConnectPass = DA_PASS;

const char *accesspointSSID = AP_SSID;
const char *accesspointPass = AP_PASS;

// Set web server port number
ESP8266WebServer server(LAMP_HTTP_PORT);
// WebSocketsServer webSocket = WebSocketsServer(LAMP_WEB_SOKET_PORT);
// WebSocketsServer webSocket(LAMP_WEB_SOKET_PORT);

// MDNSResponder MDNS;

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

void configModeCallback(WiFiManager *myWiFiManager)
{
    DPRINTLN("Entered config mode");
    DPRINTLN(WiFi.softAPIP());
    DPRINTLN(myWiFiManager->getConfigPortalSSID());
}

void setup() {
    // Serial port setup
    Serial.begin(74880);
    // Serial.begin(115200);
    Serial.println("\n\n\n+------------+");
    Serial.println("| NodeMCU v3 |");
    Serial.println("+------------+\n");

    // Mount filesystem
    DPRINT("Inizializing FS... ")
    if (LittleFS.begin()) {
        DPRINTLN("done.")
    }
    else {
        DPRINTLN("fail.")
    }

    // Initial lamp state
    lamp_state.state          = true;
    lamp_state.effect         = START_EFFECT;
    lamp_state.brightness     = BRIGHTNESS;
    lamp_state.brightness_raw = (BRIGHTNESS+1) >> 4;
    lamp_state.speed          = EFFECT_SPEED;
    lamp_state.speed_raw      = ((512-EFFECT_SPEED) >> 4) +1;
    lamp_state.scale          = SNOW_DENSE;
    if (START_EFFECT <= EFF_CODE_SNAKE )
        lamp_state.scale_raw  = lamp_state.scale;
    else
        lamp_state.scale_raw  = lamp_state.scale / 10;
    lamp_state.IP.clear();
    lamp_state.loadingFlag    = false;
    lamp_state.date_time      = &date_time;
    lamp_state.effectTimer    = &Effect_Timer;
    lamp_state.leds           = leds;

    // loadConfiguration("/config.json", lamp_state);

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
        Serial.print("Connecting to ");
        Serial.print(autoConnectSSID);
        WiFi.hostname("MagicLamp");
        wifiManager.setAPCallback(configModeCallback);
        WiFi.begin(autoConnectSSID, autoConnectPass);
        while (WiFi.status() != WL_CONNECTED) {
            delay(500);
            Serial.print(".");
        }
    }
    else {    
        Serial.print("WiFi manager ");
        wifiManager.setDebugOutput(false);
		wifiManager.autoConnect(accesspointSSID, accesspointPass);
    }
    Serial.print("connected!\nIP address: ");
    Serial.println(WiFi.localIP());
    lamp_state.IP = WiFi.localIP().toString();

    // if (MDNS.begin("MagicLamp")) {
    //     Serial.println("MDNS responder started");
    //     MDNS.addService("http", "tcp", 80);
    //     // MDNS.addService("ws", "tcp", 81);
    // } else {
    //     Serial.println("MDNS.begin failed");
    // }

    // HTTP server config
    http_server_init();
    // // SSDP descriptor
    // server.on("/description.xml", HTTP_GET, []() {
    //     DPRINTLN("/description.xml")
    //     SSDP.schema(server.client());
    // });
    // DPRINTLN("SSDP Ready!");
    // HTTP server start
    server.begin();
    DPRINTLN("HTTP server started")

//     // SSDP service
//     DPRINTLN("Starting SSDP...");
//     // String setModelURL = "http://" + lamp_state.IP;
//     // SSDP.setDeviceType("upnp:rootdevice");
//     SSDP.setSchemaURL("description.xml");
//     SSDP.setHTTPPort(80);
//     SSDP.setName("esp8266");
//     SSDP.setModelName("WeMosD1mini");
// //    SSDP.setSerialNumber(String(ESP.getChipId(),HEX));
//     SSDP.setSerialNumber("0123456789");
//     SSDP.setURL("index.html");
//     SSDP.setModelNumber("000000000001");
//     SSDP.setModelURL("http://myesp.html");
//     SSDP.setManufacturer("by Michael");
//     SSDP.setManufacturerURL("http://myesp.html");
//     SSDP.begin();


    // // WEB socket
	// webSocket.begin();
	// webSocket.onEvent(webSocketEvent);
    // DPRINTLN("Web socket server started")

    // NTP client
    timeClient.begin();

    // WDT
    ESP.wdtFeed();
    updateMode(&lamp_state);
}

void loop() {
    // MDNS.update();

    // webSocket.loop();

    server.handleClient();

    // Working with matrix
    if (lamp_state.state && Effect_Timer.isReady()) {
        SelectEffect(&lamp_state);  // Current effect drawing
        FastLED.show();             // Show matrix
    }

    // // Dawn check
    // if (Clock_Timer.isReady()) {
    //     uint32_t get_Millis;

    //     printDateTimeStruct(lamp_state.date_time);
    //     get_Millis = Clock_Timer.getMillis();
    //     DPRINTLN_FULL(get_Millis);
    //     DPRINTLN("");
    //     // lamp_state.date_time->local_time += (Clock_Timer.getMillis()-lamp_state.date_time->local_millis)/1000;

    //     //lamp_state.date_time->local_time += (Clock_Timer.getMillis()-lamp_state.date_time->local_millis)/10000;

    //     printTime(lamp_state.date_time->local_time + (Clock_Timer.getMillis()-lamp_state.date_time->local_millis)/10000);
    //     DPRINTLN("");
    // }

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
        // DPRINTLN("\nidleTimer.isReady()");
        // printTime(lamp_state.date_time->local_time + (Idle_Timer.getMillis()-lamp_state.date_time->local_millis)/1000);
        ESP.wdtFeed();
    }
}
