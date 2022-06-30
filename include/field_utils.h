CRGB parseColor(String value)
{
  uint8_t ri = value.indexOf(",");
  uint8_t gi = value.indexOf(",", ri + 1);

  String rs = value.substring(0, ri);
  String gs = value.substring(ri + 1, gi);
  String bs = value.substring(gi + 1);

  uint8_t r = rs.toInt();
  uint8_t g = gs.toInt();
  uint8_t b = bs.toInt();

  return CRGB(r, g, b);
}

void writeFieldsToEEPROM(FieldList fields, uint8_t count)
{
  uint8_t index = 25;

  EEPROM.write(index, 0);

  for (uint8_t i = 0; i < count; i++)
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

void loadFieldsFromEEPROM(FieldList fields, uint8_t count)
{
  uint8_t byteCount = 1;
  for (uint8_t i = 0; i < count; i++)
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

  for (uint8_t i = 0; i < count; i++)
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

Field getField(String name, FieldList fields, uint8_t count)
{
  for (uint8_t i = 0; i < count; i++)
  {
    Field field = fields[i];
    if (field.name == name)
    {
      return field;
    }
  }
  return Field();
}

String getFieldValue(String name, FieldList fields, uint8_t count)
{
  Field field = getField(name, fields, count);
  if (field.getValue)
  {
    return field.getValue();
  }
  return String();
}

String setFieldValue(String name, String value, FieldList fields, uint8_t count)
{
  String result;

  Field field = getField(name, fields, count);
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

  writeFieldsToEEPROM(fields, count);

  return result;
}

String getFieldOptions(String name, FieldList fields, uint8_t count)
{
  Field field = getField(name, fields, count);
  if (field.getOptions)
  {
    return field.getOptions();
  }
  else
  {
    return String();
  }
}

String getAllFields(FieldList fields, uint8_t count)
{
  StaticJsonDocument<2048> json;

  for (uint8_t i = 0; i < count; i++)
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