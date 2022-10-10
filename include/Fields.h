#ifndef FIELD_H
#define FIELD_H

#include <FastLED.h>
#include "defines.h"
#include "pattern.h"

typedef String (*FieldSetter)(String);
typedef String (*FieldGetter)();
typedef void (*FieldValueSetter)(uint8_t);
typedef uint8_t (*FieldIndexGetter)();

const String NumberFieldType = "Number";
const String SelectFieldType = "Select";
const String ColorFieldType = "Color";

typedef struct
{
public:
  String name;
  String label;
  String type;
  bool modifier;
  uint8_t min;
  uint8_t max;
  FieldGetter getValue;
  FieldGetter getOptions;
  FieldSetter setValue;
  FieldValueSetter setByValue;
  FieldIndexGetter getValueIndex;

} Field;

typedef Field FieldList[];

String getPower();
String setPower(String value);
String getBrightness();
String setBrightness(String value);
String getPattern();
uint8_t getPatternIndex();
void setPatternByValue(uint8_t value);
String setPattern(String patternName);
String getPatterns();
String getSolidColor();
String getRGBString(uint8_t r, uint8_t g, uint8_t b);
String getRGBColor(CRGB color);
String setSolidColor(String value);
String getFastLedInfo();

FieldList fields = {
    {"power", "Power", NumberFieldType, false, 0, 1, getPower, NULL, setPower},
    {"brightness", "Brightness", NumberFieldType, false, 1, 255, getBrightness, NULL, setBrightness},
    {"pattern", "Pattern", SelectFieldType, false, 0, patternCount, getPattern, getPatterns, setPattern, setPatternByValue, getPatternIndex},
    {"solidColor", "SolidColor", ColorFieldType, false, 0, 255, getSolidColor, NULL, setSolidColor},
};

uint8_t fieldCount = ARRAY_SIZE(fields);

#endif
