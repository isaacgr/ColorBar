#include <temperatures.h>
#include <pacifica.h>
#include <fire.h>

void showRGB(const CRGB &rgb)
{
  Serial.printf("R: %d\n", rgb.r);
  Serial.printf("G: %d\n", rgb.g);
  Serial.printf("B: %d\n", rgb.b);
}

/*
===============================
Solid color
===============================
*/
void showSolidColor()
{
  FastLED.setTemperature(temperatures[0].temperature);
  fill_solid(leds, NUM_LEDS, solidColor);
}

/*
===============================
Fire effect from Daves garage
===============================
*/

void DrawFireEffect()
{
  FastLED.clear();
  FastLED.setTemperature(temperatures[0].temperature);

  DrawFire(HeatColors_p);
};

/*
===============================
Water effect
===============================
*/

void DrawWaterEffect()
{
  FastLED.clear();
  FastLED.setTemperature(temperatures[0].temperature);

  DrawFire(IceColors_p);
};

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct
{
  Pattern pattern;
  String name;
  String modifiers;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

PatternAndNameList patterns = {
    {pacifica_loop, "pacifica", ""},
    {DrawFireEffect, "fire", "sparking,cooling,sparks,sparkHeight,reversed,mirrored"},
    {DrawWaterEffect, "water", "sparking,cooling,sparks,sparkHeight,reversed,mirrored"},
    {showSolidColor, "solidColor", ""},
};

uint8_t patternCount = ARRAY_SIZE(patterns);
