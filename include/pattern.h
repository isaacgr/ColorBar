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
void DrawRainbowEffect();
void DrawFillRainbowEffect();
void pacifica_loop();
void DrawFlicker1();
void DrawFlicker2();
void DrawFire(CRGBPalette16 palette);
void DrawRainbow();
void DrawFillRainbow();

// modifiers for fire and pacifica effects
extern uint8_t g_ColorTemperature = 0;
extern uint8_t g_ColorPalette = 0;
extern uint8_t g_Sparking = 150;
extern uint8_t g_Cooling = 6;
extern uint8_t g_Sparks = 1;
extern uint8_t g_SparkHeight = 1;
extern bool breversed = false;
extern bool bmirrored = true;
extern bool g_Cycle = false;
extern uint8_t g_Speed = 20;

extern PatternAndNameList patterns;
extern uint8_t patternCount;

#endif