#ifndef ROUTES_H
#define ROUTES_H

#include <FS.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <Arduino.h>
#include <WebServer.h>

void setupWeb();
String getContentType(String filename);
bool handleFileRead(String path);

extern WebServer server;

#endif