#ifndef PATTERN_H
#define PATTERN_H

#include "defines.h"

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
  String modifiers;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

void showSolidColor();
void DrawFireEffect();
void DrawWaterEffect();
void DrawRainbowEffect();
void DrawFillRainbowEffect();

PatternAndNameList patterns = {
    {showSolidColor, "solidColor", ""},
    {pacifica_loop, "pacifica", ""},
    {DrawFireEffect, "fire", ""},
    {DrawWaterEffect, "water", ""},
    {DrawRainbowEffect, "rainbow", ""},
    {DrawFillRainbowEffect, "rainbow2", ""},
    {DrawFlicker1, "flicker1", ""},
    {DrawFlicker2, "flicker2", ""},
};

uint8_t patternCount = ARRAY_SIZE(patterns);

#endif