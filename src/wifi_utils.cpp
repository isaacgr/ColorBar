#include <ESPmDNS.h>
#include <EEPROM.h>
#include <WiFi.h>
#include "eeprom_utils.h"
#include "wifi_utils.h"
#include "defines.h"
#include "secrets.h"

String hostname = "ESP-";

char *toCharArray(String str)
{
  char charArray[str.length() + 1];

  for (int i = 0; i < str.length(); i++)
  {
    charArray[i] = str[i];
  }

  return charArray;
}

String getHostname()
{
  return hostname;
}

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Connected to AP successfully!");
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info)
{
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid, password);
}

void setupWifi(int apMode)
{
  // Set Hostname.
  uint64_t chipid = ESP.getEfuseMac();
  uint16_t long1 = (unsigned long)((chipid & 0xFFFF0000) >> 16);
  uint16_t long2 = (unsigned long)((chipid & 0x0000FFFF));
  String hex = String(long1, HEX) + String(long2, HEX); // six octets
  hostname += hex;

  char hostnameChar[hostname.length() + 1];
  memset(hostnameChar, 0, hostname.length() + 1);

  for (uint8_t i = 0; i < hostname.length(); i++)
    hostnameChar[i] = hostname.charAt(i);

  WiFi.setHostname(hostnameChar);

  if (apMode != 0)
  {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(hostnameChar, WiFiAPPSK);
    Serial.printf("Connect to Wi-Fi access point: %s\n", hostnameChar);
    Serial.println("and open http://192.168.4.1 in your browser");
  }
  else
  {
    WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
    WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
    WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true);
    Serial.print("MAC >> ");
    Serial.println(WiFi.macAddress());
    Serial.println("Wait for WiFi... ");
    if (EEPROM.read(WIFI_SET) == 1)
    {
      int idIndex = EEPROM.read(SSID_INDEX);
      strcpy(ssid, toCharArray(EEPROM.readString(idIndex)));
      int passwordIndex = EEPROM.read(PASSWORD_INDEX);
      strcpy(password, toCharArray(EEPROM.readString(passwordIndex)));
    }
    WiFi.begin(ssid, password);
  }
}

void setupMDNS()
{
  const char *name;
  if (EEPROM.read(MDNS_SET) == 1)
  {
    int nameIndex = EEPROM.read(MDNS_INDEX);
    name = readString(nameIndex);
    if ((name != NULL) && (name[0] == '\0'))
    {
      name = mdns_name;
    }
  }
  else
  {
    name = mdns_name;
  }
  if (MDNS.begin(name))
  { // Start the mDNS responder for esp8266.local
    Serial.printf("mDNS responder started: %s\n", name);
  }
  else
  {
    Serial.println("Error setting up MDNS responder!");
  }
}