#ifndef __INC_WEB_FUNCTIONS_H
#define __INC_WEB_FUNCTIONS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include "debug_macros.h"
#include "functions.h"
#include "configuration_functions.h"

extern ESP8266WebServer server;
extern WebSocketsServer webSocket;
extern the_lamp_state_t lamp_state;
extern lamp_config_s lamp_config;
// File fsUploadFile;

void http_server_init();

bool handleFileRead(const String &path);

void handleFilesystemInformation();
void handleFileList();

void handleNotFound();

void handleFileLoad();
void handleFileSave();
void handleFileDelete();
void handleFileCreate();

void handleAction();
void SendLampState(the_lamp_state_t *lamp_state);

const char* getContentType(const String &filename);
String formatBytes(size_t bytes);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

#endif // __INC_WEB_FUNCTIONS_H