#include <ESPmDNS.h>
#include <EEPROM.h>
#include "defines.h"
#include "eeprom_utils.h"
#include "mdns.h"
#include "secrets.h"

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