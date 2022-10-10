#ifndef WIFI_UTILS_H
#define WIFI_UTILS_H

#include <Arduino.h>
#include <WiFi.h>

char *toCharArray(String str);
void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info);
void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info);
void setupWifi(String hostname, int apMode);

#endif