String getPower()
{
  return String(g_Power);
}

String setPower(String value)
{
  g_Power = value.toInt();
  return String(g_Power);
}

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

void setColorTemperature(uint8_t value)
{
  if (value >= colorCount)
    value = colorCount - 1;

  currentPatternIndex = value;
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

FieldList fields = {
    {"power", "Power", NumberFieldType, 0, 1, getPower, NULL, setPower},
    {"brightness", "Brightness", NumberFieldType, 1, 255, getBrightness, NULL, setBrightness},

    {"pattern", "Pattern", SelectFieldType, 0, patternCount, getPattern, getPatterns, setPattern},
    {"colorTemperature", "ColorTemperature", SelectFieldType, 0, colorCount, getColorTemperature, getColorTemperatures, setColorTemperature},
};

uint8_t fieldCount = ARRAY_SIZE(fields);