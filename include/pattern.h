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
void pacifica_loop();
void DrawFlicker1();
void DrawFlicker2();
void DrawFire(CRGBPalette16 palette);
void DrawRainbow();
void DrawFillRainbow();

// modifiers for fire, water and pacifica effects
uint8_t g_ColorTemperature = 0;
uint8_t g_ColorPalette = 0;
uint8_t g_Sparking = 150;
uint8_t g_Cooling = 6;
uint8_t g_Sparks = 1;
uint8_t g_SparkHeight = 1;
bool breversed = false;
bool bmirrored = true;
bool g_Cycle = false;
uint8_t g_Speed = 20;

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