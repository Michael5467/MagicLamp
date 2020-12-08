#ifndef __INC_WEB_FUNCTIONS_H
#define __INC_WEB_FUNCTIONS_H

#include <Arduino.h>
#include <LittleFS.h>
#include <ESP8266WebServer.h>
#include <WebSocketsServer.h>

#include "debug_macros.h"

extern ESP8266WebServer server;
extern WebSocketsServer webSocket;
// File fsUploadFile;

void http_server_init();

void handleOpenPage(String page);

void handleFilesystemInformation();
void handleFileList();
void handleNotFound();
bool handleFileRead(String path);
void handleFileUpload();
void handleFileDelete();
void handleFileCreate();
void handleAction();

String getContentType(String filename);
String formatBytes(size_t bytes);

void webSocketEvent(uint8_t num, WStype_t type, uint8_t *payload, size_t length);

#endif // __INC_WEB_FUNCTIONS_H