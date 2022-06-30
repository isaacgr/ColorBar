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

uint8_t getPatternIndex()
{
  return currentPatternIndex;
}

void setPatternByValue(uint8_t value)
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
    setPatternByValue(i);
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
    json += patterns[i].name;
    if (i < patternCount - 1)
      json += ",";
  }

  return json;
}

/*
===============================
Solid Color
===============================
*/

String getSolidColor()
{
  return String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b);
}

String setSolidColor(uint8_t r, uint8_t g, uint8_t b)
{
  solidColor = CRGB(r, g, b);

  return String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b);
}

String setSolidColor(CRGB color)
{
  return setSolidColor(color.r, color.g, color.b);
}

String setSolidColor(String value)
{
  CRGB color = parseColor(value);

  return setSolidColor(color);
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
      {"max_brightness", calculate_max_brightness_for_power_mW(255, 5 * MILLI_AMPS)},
      {"power_limit (mW)", 5 * MILLI_AMPS},
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
    {"power", "Power", NumberFieldType, false, 0, 1, getPower, NULL, setPower},
    {"brightness", "Brightness", NumberFieldType, false, 1, 255, getBrightness, NULL, setBrightness},
    {"pattern", "Pattern", SelectFieldType, false, 0, patternCount, getPattern, getPatterns, setPattern, setPatternByValue, getPatternIndex},
    {"solidColor", "SolidColor", ColorFieldType, false, 0, 255, getSolidColor, NULL, setSolidColor},
};

uint8_t fieldCount = ARRAY_SIZE(fields);