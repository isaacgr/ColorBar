bool toBoolean(String value)
{
  if (value == "0" || value == "false")
  {
    return false;
  }
  else
  {
    return true;
  }
}

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

String getPatternModifiers()
{
  return patterns[currentPatternIndex].modifiers;
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
    json += patterns[i].name;
    if (i < patternCount - 1)
      json += ",";
  }

  return json;
}

/*
===============================
Palette
===============================
*/
String getPalette()
{
  return paletteNames[currentPaletteIndex];
}

void setPalette(uint8_t value)
{
  if (value >= paletteCount)
    value = paletteCount - 1;

  currentPaletteIndex = value;
}

String setPalette(String paletteName)
{
  int i = 0;
  while (i < paletteCount)
  {
    if (paletteNames[i] == paletteName)
    {
      break;
    }
    i++;
  }
  if (i < paletteCount)
  {
    setPalette(i);
    return String(paletteNames[currentPaletteIndex]);
  }
  else
  {
    throw -1;
  }
}

String getPalettes()
{
  String json = "";

  for (uint8_t i = 0; i < paletteCount; i++)
  {
    json += paletteNames[i];
    if (i < paletteCount - 1)
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
    json += temperatures[i].name;
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
Modifiers
===============================
*/

String getCooling()
{
  return String(g_Cooling);
}
String getSparking()
{
  return String(g_Sparking);
}
String getReversed()
{
  return String(breversed);
}
String getMirrored()
{
  return String(bmirrored);
}
String getCycling()
{
  return String(g_Cycle);
}
String getSpeed()
{
  return String(g_Speed);
}
String setCooling(String value)
{
  g_Cooling = value.toInt();
  return String(g_Cooling);
}
String setSparking(String value)
{
  g_Sparking = value.toInt();
  return String(g_Sparking);
}
String setReversed(String value)
{
  breversed = toBoolean(value);
  return String(breversed);
}
String setMirrored(String value)
{
  bmirrored = toBoolean(value);
  return String(bmirrored);
}
String setCycling(String value)
{
  g_Cycle = toBoolean(value);
  return String(g_Cycle);
}
String setSpeed(String value)
{
  g_Speed = value.toInt();
  return String(g_Speed);
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

    {"pattern", "Pattern", SelectFieldType, false, 0, patternCount, getPattern, getPatterns, setPattern, getPatternModifiers},
    {"solidColor", "SolidColor", ColorFieldType, false, 0, 255, getSolidColor, NULL, setSolidColor},

    // modifiers
    {"palette", "Palette", SelectFieldType, true, 0, paletteCount, getPalette, getPalettes, setPalette},
    {"colorTemperature", "ColorTemperature", SelectFieldType, true, 0, colorCount, getColorTemperature, getColorTemperatures, setColorTemperature},
    {"cooling", "Cooling", NumberFieldType, true, 0, 255, getCooling, NULL, setCooling},
    {"sparking", "Sparking", NumberFieldType, true, 0, 255, getSparking, NULL, setSparking},
    {"reversed", "Reversed", BooleanFieldType, true, 0, 1, getReversed, NULL, setReversed},
    {"mirrored", "Mirrored", BooleanFieldType, true, 0, 1, getMirrored, NULL, setMirrored},
    {"cycling", "Cycling", BooleanFieldType, true, 0, 1, getCycling, NULL, setCycling},
    {"speed", "Speed", NumberFieldType, true, 0, 255, getSpeed, NULL, setSpeed},
};

uint8_t fieldCount = ARRAY_SIZE(fields);