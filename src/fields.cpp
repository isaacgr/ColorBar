#include <Arduino.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include "fields.h"
#include "defines.h"
#include "leds.h"
#include "pattern.h"

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

String setSolidColor(String value)
{
  solidColor = parseColor(value);
  return String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b);
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
