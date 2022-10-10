#include <FastLED.h>
#include <pattern.h>

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

// void showRGB(const CRGB &rgb)
// {
//   Serial.printf("R: %d\n", rgb.r);
//   Serial.printf("G: %d\n", rgb.g);
//   Serial.printf("B: %d\n", rgb.b);
// }

/*
===============================
Solid color
===============================
*/
void showSolidColor()
{
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
  DrawFire(IceColors_p);
};

/*
===============================
Rainbow effect
===============================
*/

void DrawRainbowEffect()
{
  FastLED.clear();
  DrawRainbow();
};

/*
===============================
Fill Rainbow effect
===============================
*/

void DrawFillRainbowEffect()
{
  FastLED.clear();
  DrawFillRainbow();
};
