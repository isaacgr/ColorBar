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
extern uint8_t g_ColorTemperature;
extern uint8_t g_ColorPalette;
extern uint8_t g_Sparking;
extern uint8_t g_Cooling;
extern uint8_t g_Sparks;
extern uint8_t g_SparkHeight;
extern bool breversed;
extern bool bmirrored;
extern bool g_Cycle;
extern uint8_t g_Speed;

extern PatternAndNameList patterns;
extern uint8_t patternCount;

#endif