/*
===============================
Power
===============================
*/
String getPower()
{
  return String(g_Power);
}

String setPower(String value)
{
  g_Power = value.toInt();
  return String(g_Power);
}

/*
===============================
Brightness
===============================
*/
String getBrightness()
{
  return String(g_Brightness);
}

String setBrightness(String value)
{
  g_Brightness = value.toInt();
  FastLED.setBrightness(g_Brightness);
  return String(g_Brightness);
}

/*
===============================
Pattern
===============================
*/
String getPattern()
{
  return patterns[currentPatternIndex].name;
}

void setPattern(uint8_t value)
{
  if (value >= patternCount)
    value = patternCount - 1;

  currentPatternIndex = value;
}

String setPattern(String patternName)
{
  int i = 0;
  while (i < patternCount)
  {
    if (patterns[i].name == patternName)
    {
      break;
    }
    i++;
  }
  if (i < patternCount)
  {
    setPattern(i);
    return String(patterns[currentPatternIndex].name);
  }
  else
  {
    throw -1;
  }
}

String getPatterns()
{
  String json = "";

  for (uint8_t i = 0; i < patternCount; i++)
  {
    json += "\"" + patterns[i].name + "\"";
    if (i < patternCount - 1)
      json += ",";
  }

  return json;
}

/*
===============================
Color Temperature
===============================
*/

void setColorTemperature(uint8_t value)
{
  if (value >= colorCount)
    value = colorCount - 1;

  currentTemperatureIndex = value;
}

String setColorTemperature(String colorName)
{
  int i = 0;
  while (i < colorCount)
  {
    if (temperatures[i].name == colorName)
    {
      break;
    }
    i++;
  }
  if (i < colorCount)
  {
    setColorTemperature(i);
    return String(temperatures[currentTemperatureIndex].name);
  }
  else
  {
    throw -1;
  }
}

String getColorTemperatures()
{
  String json = "";

  for (uint8_t i = 0; i < colorCount; i++)
  {
    json += "\"" + temperatures[i].name + "\"";
    if (i < colorCount - 1)
      json += ",";
  }

  return json;
}

String getColorTemperature()
{
  return temperatures[currentTemperatureIndex].name;
}

/*
===============================
FastLED Info
===============================
*/

String getFastLedInfo()
{
  struct Info
  {
    char name[32];
    uint32_t value;
  };

  typedef Info InfoList[];

  InfoList fastLedInfo = {
      {"fps", FastLED.getFPS()},
      {"power (mW)", calculate_unscaled_power_mW(leds, NUM_LEDS)},
      {"max_brightness", calculate_max_brightness_for_power_mW(255, g_PowerLimit)},
      {"power_limit (mW)", g_PowerLimit},
  };

  const uint8_t infoCount = ARRAY_SIZE(fastLedInfo);
  const size_t CAPACITY = JSON_ARRAY_SIZE(16);

  StaticJsonDocument<CAPACITY> doc;
  JsonArray json = doc.to<JsonArray>();
  StaticJsonDocument<256> object;

  for (int i = 0; i < infoCount; i++)
  {
    object["name"] = fastLedInfo[i].name;
    object["value"] = fastLedInfo[i].value;
    json.add(object);
  }
  String result;
  serializeJsonPretty(doc, result);
  return result;
}

FieldList fields = {
    {"power", "Power", NumberFieldType, 0, 1, getPower, NULL, setPower},
    {"brightness", "Brightness", NumberFieldType, 1, 255, getBrightness, NULL, setBrightness},

    {"pattern", "Pattern", SelectFieldType, 0, patternCount, getPattern, getPatterns, setPattern},
    {"colorTemperature", "ColorTemperature", SelectFieldType, 0, colorCount, getColorTemperature, getColorTemperatures, setColorTemperature},
};

uint8_t fieldCount = ARRAY_SIZE(fields);