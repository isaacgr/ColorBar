#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <Arduino.h>
#include "fields.h"

int writeString(uint8_t addr, const char data[]);
void writeWifiEEPROM(char ssid[], char pass[]);
void writeDeviceNameEEPROM(char name[]);
char *readString(uint8_t addr);
void writeFieldsToEEPROM(FieldList fields, uint8_t count);
void loadFieldsFromEEPROM(FieldList fields, uint8_t count);

#endif
