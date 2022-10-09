#include <EEPROM.h>
#include <stdexcept>
#include "eeprom_utils.h"
#include "defines.h"

int writeString(uint8_t addr, char data[])
{
  int _size = strlen(data);
  for (int i = 0; i < _size; i++)
  {
    EEPROM.write(addr + i, data[i]);
  }
  EEPROM.write(addr + _size, '\0'); // Add termination null character for String Data
  EEPROM.commit();
  return _size + 1;
}

char *readString(uint8_t addr)
{
  static char data[100]; // Max 100 Bytes
  int len = 0;
  unsigned char k = EEPROM.read(addr);
  while (k != '\0' && len < 500) // Read until null character
  {
    k = EEPROM.read(addr + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return data;
}

void writeWifiEEPROM(char ssid[], char pass[])
{
  int index = 100;
  if (strlen(ssid) > 50 || strlen(pass) > 50)
  {
    throw std::length_error("Cannot exceed 50 characters");
  }
  EEPROM.write(SSID_INDEX, index);
  index += writeString(index, ssid);
  EEPROM.write(PASSWORD_INDEX, index);
  index += writeString(index, pass);
  EEPROM.write(WIFI_SET, 1);
  EEPROM.commit();
}

void writeDeviceNameEEPROM(char name[])
{
  if (strlen(name) > 50)
  {
    throw std::length_error("Cannot exceed 50 characters");
  }
  int index = 202;
  EEPROM.write(MDNS_INDEX, index);
  index += writeString(index, name);
  EEPROM.write(MDNS_SET, 1);
  EEPROM.commit();
}