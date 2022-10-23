#include <EEPROM.h>
#include <FastLED.h>
#include <stdexcept>
#include "eeprom_utils.h"
#include "defines.h"
#include "leds.h"
#include "fields.h"

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

void writeFieldsToEEPROM()
{
  uint8_t index = 25;

  EEPROM.write(index, 0);

  for (uint8_t i = 0; i < fieldCount; i++)
  {
    Field field = fields[i];
    if (!field.getValue && !field.setValue)
      continue;

    if (field.type == ColorFieldType)
    {
      String value = field.getValue();
      CRGB color = parseColor(value);
      EEPROM.write(index++, color.r);
      EEPROM.write(index++, color.g);
      EEPROM.write(index++, color.b);
    }
    else
    {
      if (field.type == SelectFieldType)
      {
        byte v = field.getValueIndex();
        EEPROM.write(index++, v);
      }
      else
      {
        String value = field.getValue();
        byte v = value.toInt();
        EEPROM.write(index++, v);
      }
    }
  }

  EEPROM.commit();
}

void loadFieldsFromEEPROM()
{
  uint8_t byteCount = 1;
  for (uint8_t i = 0; i < fieldCount; i++)
  {
    Field field = fields[i];
    if (!field.setValue)
      continue;

    if (field.type == ColorFieldType)
    {
      byteCount += 3;
    }
    else
    {
      byteCount++;
    }
  }

  if (EEPROM.read(25) == 255)
  {
    Serial.println("First run, or EEPROM erased, skipping settings load!");
    return;
  }

  uint8_t index = 25;

  for (uint8_t i = 0; i < fieldCount; i++)
  {
    Field field = fields[i];
    if (!field.setValue)
      continue;

    if (field.type == ColorFieldType)
    {
      String r = String(EEPROM.read(index++));
      String g = String(EEPROM.read(index++));
      String b = String(EEPROM.read(index++));
      field.setValue(r + "," + g + "," + b);
    }
    else
    {
      byte v = EEPROM.read(index++);
      if (field.type == SelectFieldType)
      {
        field.setByValue(uint8_t(v));
      }
      else
      {
        field.setValue(String(v));
      }
    }
  }
}