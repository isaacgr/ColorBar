#ifndef FIELD_H
#define FIELD_H

#include <FastLED.h>
#include "defines.h"

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
String setSolidColor(String value);
String getFastLedInfo();

extern FieldList fields;
extern uint8_t fieldCount;

#endif
