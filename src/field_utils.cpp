#include <Arduino.h>
#include <ArduinoJson.h>
#include "fields.h"
#include "field_utils.h"
#include "eeprom_utils.h"

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

  writeFieldsToEEPROM(fields, fieldCount);

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