#include <Arduino.h>
#include <ArduinoJson.h>
#include <EEPROM.h>
#include "fields.h"
#include "field_utils.h"
#include "eeprom_utils.h"
#include "leds.h"

Field getField(String name)
{
  for (uint8_t i = 0; i < fieldCount; i++)
  {
    Field field = fields[i];
    if (field.name == name)
    {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name)
{
  Field field = getField(name);
  if (field.getValue)
  {
    return field.getValue();
  }
  return String();
}

String setFieldValue(String name, String value)
{
  String result;

  Field field = getField(name);
  if (field.setValue)
  {
    if (field.type == NumberFieldType && (value.toInt() > field.max || value.toInt() < field.min))
    {
      throw -1;
    }
    if (field.type == ColorFieldType)
    {
      // String r = server.arg("r");
      // String g = server.arg("g");
      // String b = server.arg("b");
      // String combinedValue = r + "," + g + "," + b;
      result = field.setValue(value);
    }
    else
    {
      result = field.setValue(value);
    }
  }

  writeFieldsToEEPROM();

  return result;
}

String getFieldOptions(String name)
{
  Field field = getField(name);
  if (field.getOptions)
  {
    return field.getOptions();
  }
  else
  {
    return String();
  }
}

String getAllFields()
{
  StaticJsonDocument<2048> json;

  for (uint8_t i = 0; i < fieldCount; i++)
  {
    Field field = fields[i];
    JsonObject object = json.createNestedObject();
    object["name"] = field.name;
    object["type"] = field.type;
    object["isModifier"] = field.modifier;

    if (field.getValue)
    {
      String value = field.getValue();
      object["value"] = value;
    }
    if (field.type == NumberFieldType)
    {
      object["min"] = field.min;
      object["max"] = field.max;
    }
    if (field.getOptions)
    {
      object["options"] = field.getOptions();
    }
  }

  String result;
  serializeJsonPretty(json, result);
  return result;
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