#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <FS.h>
#include <SPIFFS.h>
#include <EEPROM.h>
#include <Arduino.h>

String getContentType(String filename);
bool handleFileRead(String path);

#endif